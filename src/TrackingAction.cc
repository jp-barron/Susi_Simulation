#include "TrackingAction.hh"

#include "G4Step.hh"
#include "G4StepStatus.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeometryTolerance.hh"

#include "G4Track.hh"
#include "G4NeutrinoE.hh"

#include "G4Gamma.hh"

#include "HistoManager.hh"
#include "G4SystemOfUnits.hh"
#include "HistoManager.hh"

#include "G4VProcess.hh"
#include "G4UnitsTable.hh"
#include <cmath>

TrackingAction::TrackingAction()
		: fXstartAbs(0), fXendAbs(0), fPrimaryCharge(0)
{
}

TrackingAction::~TrackingAction()
{
}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
/*	const G4Step* aStep = aTrack->GetStep();
        G4StepPoint* preStep = aStep->GetPreStepPoint();
        G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
        G4ThreeVector pos = aTrack->GetPosition();
        G4double x = pos.x();
        G4double y = pos.y();
        G4double z = pos.z();
        G4double triggerRad = .4935 * cm;
        G4double triggerHz = 0.829 * cm;
        if ((pow(pow(x,2) + pow((z-2.233 * cm),2),1./2.) - triggerRad) < kCarTolerance && preStep->GetStepStatus()==fGeomBoundary){
                G4double totEnergy = aTrack->GetKineticEnergy();
		G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
                //analysisManager->FillH1(13,totEnergy);
	}*/
		
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{	
	const G4Step* aStep = aTrack->GetStep();
        G4StepPoint* preStep = aStep->GetPreStepPoint();
        G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();     
        G4ThreeVector pos = aTrack->GetPosition();
        G4double x = pos.x();
        G4double y = pos.y();
        G4double z = pos.z(); 
        G4double triggerRad = .4935 * cm;
        G4double triggerHz = 0.829 * cm;
        if ((((pow(pow(x,2) + pow((z-2.233 * cm),2),1./2.) - triggerRad) < kCarTolerance)|| (((std::fabs(y) - triggerHz) < kCarTolerance) && (pow(pow(x,2) + pow((z-2.233 * cm),2),1./2.) < triggerRad))) && preStep->GetStepStatus()==fGeomBoundary){

                G4double totEnergy = aTrack->GetKineticEnergy();
                G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
                analysisManager->FillH1(13,totEnergy);
	}

//	if (aTrack->GetParticleDefinition()->GetPDGCharge() == 0 && aTrack->GetCreatorProcess() != 0){
//		const G4String t = aTrack->GetCreatorProcess()->GetProcessName();
//		G4cout<<"!!! GetParticleDefinition() !"<<aTrack->GetParticleDefinition()->GetParticleType()<<G4endl;
//		G4cout<<"!!! GetProcessName() !"<<t<<G4endl;
//	}

	// delete all tracks after given position to save time
	if(aTrack->GetPosition().z() < -90.*mm)
		((G4Track*)aTrack)->SetTrackStatus(fKillTrackAndSecondaries);
}
