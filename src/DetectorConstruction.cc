#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"
#include "G4SDChargedFilter.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSTrackLength.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "PixelDetectorSD.hh"
#include "MeasureTrackAngle.hh"
#include "MeasureEnergy.hh"
#include "Trigger.hh"
#include "DetectorMessenger.hh"


// Std. sensor geometry, IBL sensor
const G4double X = 10.0 * mm;  // total sensor tile x dimension (column)
const G4double Y = 10.0 * mm;  // total sensor tile y dimension (row)
const G4double thickness = 300 * um;


G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = 0;

DetectorConstruction::DetectorConstruction() :
		G4VUserDetectorConstruction(), fCheckOverlaps(true), fWorldMaterial(0),fSolidWorld(0), fLogicWorld(0), fPhysWorld(0), fSensorMaterial(0), fSolidSensor(0), fLogicSensor(0), fPhysSensor(0), fSourceShieldMaterial(0), fSolidSourceShield(0), fLogicSourceShield(0), fPhysSourceShield(0), fCollMaterial(0), fSolidColl(0), fLogicColl(0), fPhysColl(0),fCollMountMaterial(0),fSolidCollMount(0),fLogicCollMount(0),fPhysCollMount(0),fPCBMountMaterial(0), fSolidPCBMount(0), fLogicPCBMount(0),fPhysPCBMount(0),fPCBMaterial(0),fSolidPCB(0),fLogicPCB(0),fPhysPCB(0),fTriggerMaterial(0), fSolidTrigger(0), fLogicTrigger(0), fPhysTrigger(0),fShieldMaterial(0), fSolidShield(0), fLogicShield(0), fPhysShield(0),fDetectorMessenger(0)

{
	DefineMaterials();
	SetWorldMaterial("G4_AIR");
	SetSensorMaterial("G4_Si");
	fDetectorMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
	delete fDetectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	G4cout << "DetectorConstruction::Construct()" << G4endl;

	//	Is this needed?
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();

	// World (Experimental place)
	fSolidWorld = new G4Box("World", 25. * cm / 2., 25. * cm / 2., 30. * cm / 2.);// std. dimension 15 x 15 x 20 cm
	fLogicWorld = new G4LogicalVolume(fSolidWorld, fWorldMaterial, "World");
	fPhysWorld = new G4PVPlacement(0,//no rotation
			G4ThreeVector(0, 0, 0),//at (0,0,0)
			fLogicWorld,//its logical volume
			"World",//its name
			0,//its mother  volume
			false,//no boolean operation
			0,//copy number
			fCheckOverlaps);//overlaps checking
	G4VisAttributes* worldVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 0.0, 3./4.));
	worldVisAtt->SetForceWireframe(true);
	fLogicWorld->SetVisAttributes(worldVisAtt);

	// Sensor
	fSolidSensor = new G4Box("Sensor", X / 2., Y / 2., thickness / 2.);
	fLogicSensor = new G4LogicalVolume(fSolidSensor, fSensorMaterial, "Sensor");
	fPhysSensor = new G4PVPlacement(0,//no rotation
			G4ThreeVector(0,0,-thickness / 2.),//std. position
			fLogicSensor,//its logical volume
			"Sensor",//its name
			fLogicWorld,//its mother volume
			false,//no boolean operation
			0,//copy number
			fCheckOverlaps);//overlaps checking

	fLogicSensor->SetVisAttributes(new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 1.)));

	//	Source shielding 
	//Sr90 case
	fSolidSourceShield = new G4Tubs("SourceShield", .1 * cm, 2.307 * cm,2.975 * cm, 0.*deg,360.*deg);
	//Sodium 22 case only
	//fSolidSourceShield = new G4Tubs("SourceShield", 0 * cm,1 * cm,.3 * cm, 0.*deg,360.*deg);

	fSourceShieldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_PLEXIGLASS");
	fLogicSourceShield = new G4LogicalVolume(fSolidSourceShield, fSourceShieldMaterial, "SourceShield");
	fPhysSourceShield = new G4PVPlacement(0,//no rotation
			G4ThreeVector(0, 0,-6.764 * cm),// in sr90 case//at position
			fLogicSourceShield,//its logical volume
			"SourceShield",//its name
			fLogicWorld,//its mother volume
			false,//no boolean operation
			0,//copy number
			fCheckOverlaps);//overlaps checking
	G4VisAttributes* sourceShieldVisAtt = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 4./4.));
	fLogicSourceShield->SetVisAttributes(sourceShieldVisAtt);

	//	Source collimator, one brass cylinder with small hole in the middle
	fSolidColl = new G4Tubs("Collimator",
			0.188 * cm,
			5.972 * cm / 2.,
			0.5024 * cm,
			0. * deg,
			360. * deg);
	
	fCollMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_BRASS");
	fLogicColl = new G4LogicalVolume(fSolidColl, fCollMaterial, "Collimator");
	
	fPhysColl = new G4PVPlacement(0,//no rotation
			G4ThreeVector(0, 0, -3.2855 * cm),//at position
			fLogicColl,//its logical volume
			"Collimator",//its name
			fLogicWorld,//its mother volume
			false,//no boolean operation
			0,//copy number
			fCheckOverlaps);//overlaps checking
	G4VisAttributes* collVisAtt = new G4VisAttributes(G4Colour(204./255.,204./255.,0./255., 5./5.));
	fLogicColl->SetVisAttributes(collVisAtt);


	//Plastic mount for collimator
	G4Box *CollMountBox = new G4Box("Collimator mount",8.115 * cm / 2.,13.068 * cm / 2., .542 * cm / 2.);//Rectangular box
	G4Tubs *CollMountHole = new G4Tubs("Hole in collimator mount", 0, 0.271 * cm, .545 * cm / 2., 0. * deg, 360. * deg);//hole in box
	G4ThreeVector collmount_translate(0,-2.448 * cm, 0);//off-centred hole
	fSolidCollMount = new G4SubtractionSolid("Collimator mount with hole",CollMountBox,CollMountHole,0,collmount_translate);//cut hole out of box. 

	fCollMountMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_PLEXIGLASS");
	fLogicCollMount = new G4LogicalVolume(fSolidCollMount,fCollMountMaterial,"Collimator mount with hole");
	
	fPhysCollMount = new G4PVPlacement(0,
			     G4ThreeVector(0,2.448 * cm,-2.513*cm),
			     fLogicCollMount,
			     "Collimator mount with hole",
			     fLogicWorld,
			     false,
			     0,
		  	     fCheckOverlaps);
	G4VisAttributes* collmountVisAtt = new G4VisAttributes(G4Colour(20./255.,20./255.,20./255.,1./5.));
	fLogicCollMount->SetVisAttributes(collmountVisAtt);

	//Aluminum holder for PCB holding sensor. Complicated shape.
	G4Box *ProngBox = new G4Box("box for prongs of aluminum pcb holder",5.559 * cm /2., 4.033 * cm /2., 0.91 * cm / 2.);//Will be sliced into two using the next box
	G4Box *ProngGap = new G4Box("gap between prongs of aluminum pcb holder",2.359 * cm / 2., 4.034 * cm / 2., 0.92 * cm / 2.);
	G4SubtractionSolid *Prongs = new G4SubtractionSolid("prongs of pcb holder", ProngBox, ProngGap);
	G4Box *BracketBox = new G4Box("box for aluminum bracket",10.0 * cm / 2., 10.0 * cm / 2., .503 * cm / 2.);
	G4Box *BracketHole = new G4Box("hole in aluminum bracket",5.91 * cm / 2., 7.502 * cm / 2., .504 * cm / 2.);
	G4ThreeVector bracket_translate(0,-1.25 * cm, 0);
	G4SubtractionSolid *Bracket = new G4SubtractionSolid("bracket of pcb holder", BracketBox, BracketHole,0,bracket_translate);
	G4ThreeVector prong_translate(0,7.015 * cm,-0.20 * cm);
	fSolidPCBMount = new G4UnionSolid("Aluminum PCB holder",Bracket,Prongs,0,prong_translate);

	fPCBMountMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
	fLogicPCBMount = new G4LogicalVolume(fSolidPCBMount,fPCBMountMaterial,"Aluminum PCB holder");

	fPhysPCBMount = new G4PVPlacement(0,
			    G4ThreeVector(0,-7.015 * cm, .735 * cm),
			    fLogicPCBMount,
			    "Aluminum PCB holder",
			    fLogicWorld,
			    false,
			    0,
			    fCheckOverlaps);
	G4VisAttributes* PCBMountVisAtt = new G4VisAttributes(G4Colour(128./255.,128./255.,128./255.,5./5.));
	fLogicPCBMount->SetVisAttributes(PCBMountVisAtt);
	//Actual PCB

	fPCBMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");
	G4Box *PCBbox = new G4Box("PCB box",3.404 * cm / 2., 3.714 * cm / 2., 0.08 * cm / 2.);
	G4Tubs *PCBhole = new G4Tubs("PCB hole", 0,0.20 * cm, 0.081 * cm / 2.,0.* deg,360. * deg);
	G4ThreeVector pcbtranslate(0,0,0);
	fSolidPCB = new G4SubtractionSolid("PCB",PCBbox,PCBhole,0,pcbtranslate);
	fLogicPCB = new G4LogicalVolume(fSolidPCB,fPCBMaterial,"PCB");
	fPhysPCB = new G4PVPlacement(0,G4ThreeVector(0,0,.04 * cm),fLogicPCB,"PCB",fLogicWorld,false,0,fCheckOverlaps);
	G4VisAttributes* PCBVisAtt = new G4VisAttributes(G4Colour(255./255.,255./255.,210./255.,5./5.));
	fLogicPCB->SetVisAttributes(PCBVisAtt);

	//	Trigger. 
	fSolidTrigger = new G4Tubs("Trigger", 0. * cm, 0.987 * cm / 2., 1.658 * cm / 2., 0. * deg, 360. * deg);
	fTriggerMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE");
	fLogicTrigger = new G4LogicalVolume(fSolidTrigger, fTriggerMaterial, "Trigger");
	G4RotationMatrix* rotm  = new G4RotationMatrix();
    	rotm->rotateX(90*deg);
	fPhysTrigger = new G4PVPlacement(rotm,//rotation
			G4ThreeVector(0, 0,2.233 * cm),// 2.233 * cm),//at position
			fLogicTrigger,//its logical volume
			"Trigger",//its name
			fLogicWorld,//its mother volume
			false,//no boolean operation
			0,//copy number
			fCheckOverlaps);//overlaps checking
	G4VisAttributes* triggerVisAtt = new G4VisAttributes(G4Colour(240./255.,240./255.,222./255., 5./5.));
	fLogicTrigger->SetVisAttributes(triggerVisAtt);

/*	//	Setup shielding - repurposed as piece of plastic below sensor. 
	fSolidShield = new G4Box("Shield", 3. * cm / 2., 4. * cm / 2., .015 * cm / 2.);
	fShieldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_PLEXIGLASS");
	fLogicShield = new G4LogicalVolume(fSolidShield, fShieldMaterial, "Shield");
	fPhysShield = new G4PVPlacement(0,//no rotation
			G4ThreeVector(0, 0, 3.0 * cm),//at position
			fLogicShield,//its logical volume
			"Shield",//its name
			fLogicWorld,//its mother volume
			false,//no boolean operation
			0,//copy number
			fCheckOverlaps);//overlaps checking
	G4VisAttributes* shieldVisAtt = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
	fLogicShield->SetVisAttributes(shieldVisAtt);
*/
	return fPhysWorld;
}

void DetectorConstruction::ConstructSDandField()
{
	G4cout << " DetectorConstruction::ConstructSDandField()" << G4endl;
	G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

	G4MultiFunctionalDetector* siliconDetector = new G4MultiFunctionalDetector("Detector");
	DefineSensorScorers(siliconDetector);
	G4SDManager::GetSDMpointer()->AddNewDetector(siliconDetector);
	fLogicSensor->SetSensitiveDetector(siliconDetector);

	G4MultiFunctionalDetector* triggerDetector = new G4MultiFunctionalDetector("Trigger");
	DefineTriggerScorers(triggerDetector);
	G4SDManager::GetSDMpointer()->AddNewDetector(triggerDetector);
	fLogicTrigger->SetSensitiveDetector(triggerDetector);

	G4MultiFunctionalDetector* sourceshielding = new G4MultiFunctionalDetector("SourceShield");
	DefineShieldingScorers(sourceshielding);
	G4SDManager::GetSDMpointer()->AddNewDetector(sourceshielding);
	fLogicSourceShield->SetSensitiveDetector(sourceshielding);
}

G4double DetectorConstruction::GetSensorThickness()
{
	if (fSolidSensor == 0)  // sensor not contructed yet
		return thickness;
	return fSolidSensor->GetZHalfLength() * 2.;
}

void DetectorConstruction::test()
{
	if (fPhysTrigger->GetMotherLogical() == fLogicWorld) {
		G4cout << "DEACTIVATE TRIGGER" << G4endl;
		fLogicWorld->RemoveDaughter(fPhysTrigger);
		fPhysTrigger->SetMotherLogical(0);
	}
	else {
		fLogicWorld->AddDaughter(fPhysTrigger);
		fPhysTrigger->SetMotherLogical(fLogicWorld);
		G4cout<<"ACTIVATE TRIGGER"<<G4endl;
	}
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::DefineMaterials()
{
	new G4Material("Galactic", 1., 1.008 * g / mole, 1.e-25 * g / cm3, kStateGas, 2.73 * kelvin, 3.e-18 * pascal);
	new G4Material("Gold", 79., 196.97 * g / mole, 19.32 * g / cm3);
	G4NistManager* nistManager = G4NistManager::Instance();
	G4Material* StainlessSteel = new G4Material("StainlessSteel", 8.06 * g / cm3, 6);
	StainlessSteel->AddElement(nistManager->FindOrBuildElement("C"), 0.001);
	StainlessSteel->AddElement(nistManager->FindOrBuildElement("Si"), 0.007);
	StainlessSteel->AddElement(nistManager->FindOrBuildElement("Cr"), 0.18);
	StainlessSteel->AddElement(nistManager->FindOrBuildElement("Mn"), 0.01);
	StainlessSteel->AddElement(nistManager->FindOrBuildElement("Fe"), 0.712);
	StainlessSteel->AddElement(nistManager->FindOrBuildElement("Ni"), 0.09);
}

void DetectorConstruction::SetSensorMaterial(G4String materialChoice)
{
	G4Material* newMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

	if (newMaterial && fSensorMaterial != newMaterial) {
		fSensorMaterial = newMaterial;
		if (fLogicSensor)
			fLogicSensor->SetMaterial(fSensorMaterial);
		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "Sensor material set to " << materialChoice << G4endl;
	}
	if (newMaterial == 0) {
		G4ExceptionDescription msg;
		msg << "Material " << materialChoice << " not known";
		G4Exception("DetectorConstruction", "Please select a known material.", JustWarning, msg);
	}
}

void DetectorConstruction::SetWorldMaterial(G4String materialChoice)
{
	G4Material* newMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

	if (newMaterial && fWorldMaterial != newMaterial) {
		fWorldMaterial = newMaterial;
		if (fLogicWorld)
			fLogicWorld->SetMaterial(fWorldMaterial);
		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "World material set to " << materialChoice << G4endl;
	}
	if (newMaterial == 0) {
		G4ExceptionDescription msg;
		msg << "Material " << materialChoice << " not known";
		G4Exception("DetectorConstruction", "Please select a known material.", JustWarning, msg);
	}
}

void DetectorConstruction::SetWorldSizeZ(G4double val)
{
	fSolidWorld->SetZHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "World z size set to " << G4BestUnit(val, "Length") << G4endl;
}

G4double DetectorConstruction::GetWorldSizeXY()
{
	return fSolidWorld->GetXHalfLength() * 2.;
}

G4double DetectorConstruction::GetWorldSizeZ()
{
	return fSolidWorld->GetZHalfLength() * 2.;
}

void DetectorConstruction::SetWorldSizeXY(G4double val)
{
	fSolidWorld->SetXHalfLength(val / 2.);
	fSolidWorld->SetYHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "World x,y size set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetSensorPos(G4ThreeVector pos)
{
	fPhysSensor->SetTranslation(pos);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor position set to " << pos << G4endl;
}

void DetectorConstruction::SetSensorRot(G4double phi)
{
	// u, v, w are the daughter axes, projected on the mother frame
	G4ThreeVector u = G4ThreeVector(1, 0, 0);
	G4ThreeVector v = G4ThreeVector(0., std::cos(phi), -std::sin(phi));
	G4ThreeVector w = G4ThreeVector(0, std::sin(phi), std::cos(phi));
	G4RotationMatrix* rotm1 = new G4RotationMatrix(u, v, w);
	fPhysSensor->SetRotation(rotm1);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor rotated to phi = " << phi << G4endl;
}

void DetectorConstruction::SetSensorThickness(G4double val)
{
	fSolidSensor->SetZHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor thickness set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetSensorSizeX(G4double val)
{
	fSolidSensor->SetXHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor size x set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetSensorSizeY(G4double val)
{
	fSolidSensor->SetYHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor size y set to " << G4BestUnit(val, "Length") << G4endl;
}

G4double DetectorConstruction::GetSensorSizeX()
{
	if (fSolidSensor == 0)
		return X;
	return fSolidSensor->GetXHalfLength() * 2.;
}

G4double DetectorConstruction::GetSensorSizeY()
{
	if (fSolidSensor == 0)
		return Y;
	return fSolidSensor->GetYHalfLength() * 2.;
}

void DetectorConstruction::SetTrigger(const bool& value)
{
	if (!value && fPhysTrigger->GetMotherLogical() == fLogicWorld) {
		fLogicWorld->RemoveDaughter(fPhysTrigger);
		fPhysTrigger->SetMotherLogical(0);
		G4cout << "DetectorConstruction: deactivate trigger" << G4endl;
	}
	else if (value && fPhysTrigger->GetMotherLogical() == 0) {
		fLogicWorld->AddDaughter(fPhysTrigger);
		fPhysTrigger->SetMotherLogical(fLogicWorld);
		G4cout << "DetectorConstruction: activate trigger" << G4endl;
	}
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetTriggerMaterial(G4String materialChoice)
{
	G4Material* newMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
	if (newMaterial && fTriggerMaterial != newMaterial) {
		fTriggerMaterial = newMaterial;
		if (fLogicTrigger)
			fLogicTrigger->SetMaterial(fTriggerMaterial);
		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "Trigger material set to " << materialChoice << G4endl;
	}
	if (newMaterial == 0) {
		G4ExceptionDescription msg;
		msg << "Material " << materialChoice << " not known";
		G4Exception("DetectorConstruction", "Please select a known material.", JustWarning, msg);
	}
}

void DetectorConstruction::SetTriggerPos(G4ThreeVector pos)
{
	fPhysTrigger->SetTranslation(pos);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Trigger position set to " << G4BestUnit(pos, "Length") << G4endl;
}

void DetectorConstruction::SetTriggerRadius(G4double val)
{
	fSolidTrigger->SetOuterRadius(val);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Trigger radius set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetTriggerLength(G4double val)
{
	fSolidTrigger->SetZHalfLength(val / 2.);
	
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Trigger length set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetCollimator(const bool& value)
{
	if (!value && fPhysColl->GetMotherLogical() == fLogicWorld) {
		fLogicWorld->RemoveDaughter(fPhysColl);
		fPhysColl->SetMotherLogical(0);
		G4cout << "DetectorConstruction: deactivate collimator" << G4endl;
	}
	else if (value && fPhysColl->GetMotherLogical() == 0) {
		fLogicWorld->AddDaughter(fPhysColl);
		fPhysColl->SetMotherLogical(fLogicWorld);
		G4cout << "DetectorConstruction: activate collimator" << G4endl;
	}
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetCollMaterial(G4String materialChoice)
{
	G4Material* newMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
	if (newMaterial && fCollMaterial != newMaterial) {
		fCollMaterial = newMaterial;
		if (fLogicColl)
			fLogicColl->SetMaterial(fCollMaterial);
		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "Collimator material set to " << materialChoice << G4endl;
	}
	if (newMaterial == 0) {
		G4ExceptionDescription msg;
		msg << "Material " << materialChoice << " not known";
		G4Exception("DetectorConstruction", "Please select a known material.", JustWarning, msg);
	}
}


void DetectorConstruction::SetCollInnerRadius(G4double radius)
{
	fSolidColl->SetInnerRadius(radius);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Collimator inner radius set to " << G4BestUnit(radius, "Length") << G4endl;
}



void DetectorConstruction::SetCollOuterRadius(G4double radius)
{
	fSolidColl->SetOuterRadius(radius);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Collimator outer radius set to " << G4BestUnit(radius, "Length") << G4endl;
}

void DetectorConstruction::SetCollThickness(G4double val)
{
	fSolidColl->SetZHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Set collimator thickness set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetCollPos(G4ThreeVector pos)
{
	fPhysColl->SetTranslation(pos);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Collimator position set to " << G4BestUnit(pos, "Length") << G4endl;
}

void DetectorConstruction::SetSourceShield(const bool& value)
{
	if (!value && fPhysSourceShield->GetMotherLogical() == fLogicWorld) {
		fLogicWorld->RemoveDaughter(fPhysSourceShield);
		fPhysSourceShield->SetMotherLogical(0);
		G4cout << "DetectorConstruction: deactivate SourceShield" << G4endl;
	}
	else if (value && fPhysSourceShield->GetMotherLogical() == 0) {
		fLogicWorld->AddDaughter(fPhysSourceShield);
		fPhysSourceShield->SetMotherLogical(fLogicWorld);
		G4cout << "DetectorConstruction: activate SourceShield" << G4endl;
	}
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetSourceShieldMaterial(G4String materialChoice)
{
	G4Material* newMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
	if (newMaterial && fSourceShieldMaterial != newMaterial) {
		fSourceShieldMaterial = newMaterial;
		if (fLogicSourceShield)
			fLogicSourceShield->SetMaterial(fSourceShieldMaterial);
		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "SourceShield material set to " << materialChoice << G4endl;
	}
	if (newMaterial == 0) {
		G4ExceptionDescription msg;
		msg << "Material " << materialChoice << " not known";
		G4Exception("DetectorConstruction", "Please select a known material.", JustWarning, msg);
	}
}

void DetectorConstruction::SetSourceShieldPos(G4ThreeVector pos)
{
	fPhysSourceShield->SetTranslation(pos);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "SourceShield position set to " << G4BestUnit(pos, "Length") << G4endl;
}

void DetectorConstruction::SetSourceShieldThickness(G4double val)
{
	fSolidSourceShield->SetZHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "SourceShield thickness set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetSourceShieldRadius(G4double val)
{
	fSolidSourceShield->SetOuterRadius(val);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "SourceShield size radius set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetShield(const bool& value)
{
	if (!value && fPhysShield->GetMotherLogical() == fLogicWorld) {
		fLogicWorld->RemoveDaughter(fPhysShield);
		fPhysShield->SetMotherLogical(0);
		G4cout << "DetectorConstruction: deactivate Shield" << G4endl;
	}
	else if (value && fPhysShield->GetMotherLogical() == 0) {
		fLogicWorld->AddDaughter(fPhysShield);
		fPhysShield->SetMotherLogical(fLogicWorld);
		G4cout << "DetectorConstruction: activate Shield" << G4endl;
	}
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetShieldMaterial(G4String materialChoice)
{
	G4Material* newMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
	if (newMaterial && fShieldMaterial != newMaterial) {
		fShieldMaterial = newMaterial;
		if (fLogicShield)
			fLogicShield->SetMaterial(fShieldMaterial);
		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "Shield material set to " << materialChoice << G4endl;
	}
	if (newMaterial == 0) {
		G4ExceptionDescription msg;
		msg << "Material " << materialChoice << " not known";
		G4Exception("DetectorConstruction", "Please select a known material.", JustWarning, msg);
	}
}

void DetectorConstruction::SetShieldPos(G4ThreeVector pos)
{
	fPhysShield->SetTranslation(pos);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Shield position set to " << G4BestUnit(pos, "Length") << G4endl;
}

void DetectorConstruction::SetShieldThickness(G4double val)
{
	fSolidShield->SetZHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Shield thickness set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::SetShieldSizeXY(G4double val)
{
	fSolidShield->SetXHalfLength(val / 2.);
	fSolidShield->SetYHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Shield size x,y set to " << G4BestUnit(val, "Length") << G4endl;
}

void DetectorConstruction::DefineSensorScorers(G4MultiFunctionalDetector* sensor)
{
	G4VPrimitiveScorer* primitive;
	G4SDChargedFilter* charged = new G4SDChargedFilter("chargedFilter");

	primitive = new G4PSEnergyDeposit("EnergyDeposit");
	sensor->RegisterPrimitive(primitive);

	primitive = new G4PSTrackLength("TrackLength");
	primitive->SetFilter(charged);
	sensor->RegisterPrimitive(primitive);

	primitive = new MeasureTrackAngle("TrackAngleIn", fFlux_In);
	primitive->SetFilter(charged);
	sensor->RegisterPrimitive(primitive);

	primitive = new MeasureTrackAngle("TrackAngleOut", fFlux_Out);
	primitive->SetFilter(charged);
	sensor->RegisterPrimitive(primitive);
}

void DetectorConstruction::DefineTriggerScorers(G4MultiFunctionalDetector* trigger)
{
	G4VPrimitiveScorer* primitive;
	G4SDChargedFilter* charged = new G4SDChargedFilter("chargedFilter");

	primitive = new G4PSEnergyDeposit("EnergyDeposit");
        trigger->RegisterPrimitive(primitive);


	primitive = new Trigger("TriggerMechanism");
	primitive->SetFilter(charged);
	trigger->RegisterPrimitive(primitive);
}

void DetectorConstruction::DefineShieldingScorers(G4MultiFunctionalDetector* trigger)
{
	G4VPrimitiveScorer* primitive;
	G4SDChargedFilter* charged = new G4SDChargedFilter("chargedFilter");

	primitive = new MeasureEnergy("MeasureEnergyIn", fFlux_In);
	primitive->SetFilter(charged);
	trigger->RegisterPrimitive(primitive);

	primitive = new MeasureEnergy("MeasureEnergyOut", fFlux_Out);
	primitive->SetFilter(charged);
	trigger->RegisterPrimitive(primitive);
}

