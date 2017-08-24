#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"

class G4Material;
class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;
class DetectorMessenger;
class G4MultiFunctionalDetector;

class DetectorConstruction: public G4VUserDetectorConstruction {
	public:
		DetectorConstruction();
		virtual ~DetectorConstruction();

	public:
		virtual G4VPhysicalVolume* Construct();
		virtual void ConstructSDandField();
		// experimental hall (world)
		void SetWorldMaterial(G4String);
		void SetWorldSizeXY(G4double);
		void SetWorldSizeZ(G4double);
		G4double GetWorldSizeXY();
		G4double GetWorldSizeZ();
		// sensor
		void SetSensorMaterial(G4String);
		void SetSensorThickness(G4double);
		void SetSensorSizeX(G4double);
		void SetSensorSizeY(G4double);
		void SetSensorPos(G4ThreeVector pos);
		void SetSensorRot(G4double phi);
		G4double GetSensorSizeX();
		G4double GetSensorSizeY();
		G4double GetSensorThickness();
		// trigger
		void SetTrigger(const bool&);
		void SetTriggerMaterial(G4String);
		void SetTriggerLength(G4double);
		void SetTriggerRadius(G4double);
		void SetTriggerPos(G4ThreeVector pos);
		// collimator
		void SetCollimator(const bool&);
		void SetCollMaterial(G4String);
		void SetCollInnerRadius(G4double);
		void SetCollOuterRadius(G4double);
		void SetCollThickness(G4double);
		void SetCollPos(G4ThreeVector pos);
		// source shield (material between source and sensor)
		void SetSourceShield(const bool&);
		void SetSourceShieldMaterial(G4String);
		void SetSourceShieldThickness(G4double);
		void SetSourceShield(G4double);
		void SetSourceShieldPos(G4ThreeVector pos);
		void SetSourceShieldRadius(G4double val);
		// shield (material behind sensor for backscattering)
		void SetShield(const bool&);
		void SetShieldMaterial(G4String);
		void SetShieldThickness(G4double);
		void SetShieldSizeXY(G4double);
		void SetShieldPos(G4ThreeVector pos);

		void test();

	private:
		void DefineMaterials();
		G4VPhysicalVolume* DefineVolumes();
		void DefineSensorScorers(G4MultiFunctionalDetector* sensor);
		void DefineTriggerScorers(G4MultiFunctionalDetector* trigger);
		void DefineShieldingScorers(G4MultiFunctionalDetector* trigger);

		static G4ThreadLocal G4GlobalMagFieldMessenger* fMagFieldMessenger;

		G4bool fCheckOverlaps; // option to activate checking of volumes overlaps

		G4Material* fWorldMaterial;
		G4Box* fSolidWorld;
		G4LogicalVolume* fLogicWorld;
		G4VPhysicalVolume* fPhysWorld;

		G4Material* fSensorMaterial;
		G4Box* fSolidSensor;
		G4LogicalVolume* fLogicSensor;
		G4VPhysicalVolume* fPhysSensor;

		G4Material* fSourceShieldMaterial;
                G4Tubs* fSolidSourceShield;
                G4LogicalVolume* fLogicSourceShield;
                G4VPhysicalVolume* fPhysSourceShield;

		G4Material* fCollMaterial;
                G4Tubs* fSolidColl;
                G4LogicalVolume* fLogicColl;
                G4VPhysicalVolume* fPhysColl;

		G4Material* fCollMountMaterial;
                G4SubtractionSolid* fSolidCollMount;
                G4LogicalVolume* fLogicCollMount;
                G4VPhysicalVolume* fPhysCollMount;		

		G4Material* fPCBMountMaterial;
                G4UnionSolid* fSolidPCBMount;
                G4LogicalVolume* fLogicPCBMount;
                G4VPhysicalVolume* fPhysPCBMount;

		G4Material* fPCBMaterial;
                G4SubtractionSolid* fSolidPCB;
                G4LogicalVolume* fLogicPCB;
                G4VPhysicalVolume* fPhysPCB;


		G4Material* fTriggerMaterial;
		G4Tubs* fSolidTrigger;
		G4LogicalVolume* fLogicTrigger;
		G4VPhysicalVolume* fPhysTrigger;

		G4Material* fShieldMaterial;
		G4Box* fSolidShield;
		G4LogicalVolume* fLogicShield;
		G4VPhysicalVolume* fPhysShield;
		

		DetectorMessenger* fDetectorMessenger;
};

#endif

