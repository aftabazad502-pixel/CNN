#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include <cstdlib>
#include <iostream>

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(),
    fTopSensorLog(nullptr), fBottomSensorLog(nullptr) {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    // ── Read geometry ID from environment variable ──────────────
    int geomID = 1; // default: G1 baseline
    const char* geomEnv = std::getenv("GEOM_ID");
    if (geomEnv) {
        geomID = std::atoi(geomEnv);
        if (geomID < 1 || geomID > 7) {
            G4cerr << "WARNING: Invalid GEOM_ID=" << geomID
                   << ", defaulting to 1 (baseline)" << G4endl;
            geomID = 1;
        }
    }
    G4cout << "========================================" << G4endl;
    G4cout << "  GEOMETRY CONFIGURATION: G" << geomID << G4endl;
    G4cout << "========================================" << G4endl;

    // ── Materials ───────────────────────────────────────────────
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* air      = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* concrete = nist->FindOrBuildMaterial("G4_CONCRETE");
    G4Material* scint    = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    G4Material* steel    = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");

    // ── World volume (4m × 4m × 4m air) ────────────────────────
    G4double world_sizeXY = 4*m, world_sizeZ = 4*m;
    G4Box* solidWorld = new G4Box("World",
        0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);
    G4LogicalVolume* logicWorld =
        new G4LogicalVolume(solidWorld, air, "World");
    G4VPhysicalVolume* physWorld =
        new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World",
                          0, false, 0, true);

    // ── Bridge volume (2m × 2m × 1m concrete at origin) ────────
    G4double bridge_x = 2*m, bridge_y = 2*m, bridge_z = 1*m;
    G4Box* solidBridge = new G4Box("Bridge",
        0.5*bridge_x, 0.5*bridge_y, 0.5*bridge_z);
    G4LogicalVolume* logicBridge =
        new G4LogicalVolume(solidBridge, concrete, "Bridge");
    new G4PVPlacement(0, G4ThreeVector(0,0,0), logicBridge, "Bridge",
                      logicWorld, false, 0, true);

    // ── Geometry-specific void/defect placement ─────────────────
    switch (geomID) {

    case 1: {
        // G1: BASELINE — single 50cm void at (200mm, 100mm, 0)
        G4cout << "  G1: Single 50cm void at (200,100,0) mm" << G4endl;
        G4double vs = 0.5*m;
        G4Box* solidVoid = new G4Box("Void", 0.5*vs, 0.5*vs, 0.5*vs);
        G4LogicalVolume* logicVoid =
            new G4LogicalVolume(solidVoid, air, "Void");
        new G4PVPlacement(0, G4ThreeVector(200*mm, 100*mm, 0),
                          logicVoid, "Void", logicBridge, false, 0, true);
        break;
    }

    case 2: {
        // G2: Small void — 10cm cube at (200mm, 100mm, 0)
        G4cout << "  G2: Small 10cm void at (200,100,0) mm" << G4endl;
        G4double vs = 0.1*m;
        G4Box* solidVoid = new G4Box("Void", 0.5*vs, 0.5*vs, 0.5*vs);
        G4LogicalVolume* logicVoid =
            new G4LogicalVolume(solidVoid, air, "Void");
        new G4PVPlacement(0, G4ThreeVector(200*mm, 100*mm, 0),
                          logicVoid, "Void", logicBridge, false, 0, true);
        break;
    }

    case 3: {
        // G3: Two voids — 50cm cubes at (-300,-200,0) and (300,200,0)
        G4cout << "  G3: Two 50cm voids at (-300,-200,0) and (300,200,0) mm"
               << G4endl;
        G4double vs = 0.5*m;
        G4Box* solidVoid1 = new G4Box("Void1", 0.5*vs, 0.5*vs, 0.5*vs);
        G4LogicalVolume* logicVoid1 =
            new G4LogicalVolume(solidVoid1, air, "Void1");
        new G4PVPlacement(0, G4ThreeVector(-300*mm, -200*mm, 0),
                          logicVoid1, "Void1", logicBridge, false, 0, true);

        G4Box* solidVoid2 = new G4Box("Void2", 0.5*vs, 0.5*vs, 0.5*vs);
        G4LogicalVolume* logicVoid2 =
            new G4LogicalVolume(solidVoid2, air, "Void2");
        new G4PVPlacement(0, G4ThreeVector(300*mm, 200*mm, 0),
                          logicVoid2, "Void2", logicBridge, false, 1, true);
        break;
    }

    case 4: {
        // G4: Off-centre void — 50cm cube at (600mm, 500mm, 0)
        G4cout << "  G4: Off-centre 50cm void at (600,500,0) mm" << G4endl;
        G4double vs = 0.5*m;
        G4Box* solidVoid = new G4Box("Void", 0.5*vs, 0.5*vs, 0.5*vs);
        G4LogicalVolume* logicVoid =
            new G4LogicalVolume(solidVoid, air, "Void");
        new G4PVPlacement(0, G4ThreeVector(600*mm, 500*mm, 0),
                          logicVoid, "Void", logicBridge, false, 0, true);
        break;
    }

    case 5: {
        // G5: No void — solid concrete, nothing to place
        G4cout << "  G5: Solid concrete (no void)" << G4endl;
        break;
    }

    case 6: {
        // G6: Crack void — thin 400mm × 50mm × 400mm at (200,100,0)
        G4cout << "  G6: Crack 400x50x400 mm at (200,100,0) mm" << G4endl;
        G4Box* solidCrack = new G4Box("Crack",
            0.5*400*mm, 0.5*50*mm, 0.5*400*mm);
        G4LogicalVolume* logicCrack =
            new G4LogicalVolume(solidCrack, air, "Crack");
        new G4PVPlacement(0, G4ThreeVector(200*mm, 100*mm, 0),
                          logicCrack, "Crack", logicBridge, false, 0, true);
        break;
    }

    case 7: {
        // G7: Rebar grid (3 steel bars, 20mm diameter, along X)
        //     + 50cm void at (200, 100, 0)
        G4cout << "  G7: Rebar grid + 50cm void" << G4endl;

        // Void
        G4double vs = 0.5*m;
        G4Box* solidVoid = new G4Box("Void", 0.5*vs, 0.5*vs, 0.5*vs);
        G4LogicalVolume* logicVoid =
            new G4LogicalVolume(solidVoid, air, "Void");
        new G4PVPlacement(0, G4ThreeVector(200*mm, 100*mm, 0),
                          logicVoid, "Void", logicBridge, false, 0, true);

        // Rebar: 3 cylindrical bars along X axis at different Y positions
        // Each bar: radius=10mm, half-length = bridge_x/2 = 1m
        // Rotation: default G4Tubs axis is Z, rotate 90° around Y to align
        //           with X axis
        G4RotationMatrix* rebarRot = new G4RotationMatrix();
        rebarRot->rotateY(90*deg);

        G4double rebarRadius = 10*mm;
        G4double rebarHalfLen = 0.5*bridge_x; // 1m half-length
        G4Tubs* solidRebar = new G4Tubs("Rebar",
            0, rebarRadius, rebarHalfLen, 0, 360*deg);

        // Bar positions: Y = -400mm, 0, +400mm; Z = -200mm (lower third)
        G4double rebarZ = -200*mm;
        G4double rebarY[3] = {-400*mm, 0*mm, 400*mm};

        for (int i = 0; i < 3; i++) {
            G4String name = "Rebar_" + std::to_string(i);
            G4LogicalVolume* logicRebar =
                new G4LogicalVolume(solidRebar, steel, name);
            new G4PVPlacement(rebarRot,
                G4ThreeVector(0, rebarY[i], rebarZ),
                logicRebar, name, logicBridge, false, i, true);
        }
        break;
    }

    default:
        G4cerr << "ERROR: Unknown GEOM_ID=" << geomID << G4endl;
        break;
    }

    // ── Sensor planes (same for all geometries) ─────────────────
    G4double sensor_x = 2*m, sensor_y = 2*m, sensor_z = 2*cm;
    G4Box* solidSensor = new G4Box("Sensor",
        0.5*sensor_x, 0.5*sensor_y, 0.5*sensor_z);

    fTopSensorLog = new G4LogicalVolume(solidSensor, scint, "TopSensor");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 1.0*m),
                      fTopSensorLog, "TopSensor", logicWorld, false, 0, true);

    fBottomSensorLog = new G4LogicalVolume(solidSensor, scint, "BottomSensor");
    new G4PVPlacement(0, G4ThreeVector(0, 0, -1.0*m),
                      fBottomSensorLog, "BottomSensor", logicWorld, false, 0, true);

    return physWorld;
}
