#ifndef RootAnalysis_HZZAnalyzer_H
#define RootAnalysis_HZZAnalyzer_H

#include <string>
#include <vector>
#include <map>

#include "ObjectMessenger.h"
#include "EventProxyBase.h"
#include "EventProxyHTT.h"

#include "strbitset.h"
#include "TDirectory.h"

//ROOT includes
#include "TTree.h"
#include "TList.h"

#include "Analyzer.h"

class HZZHistograms;

class TH1F;
class TH2F;
class TH3F;
class TLorentzVector;


  typedef struct {
  ///Variables saved into a TTree
  float mass4Mu;
  float massZ1, massZ2;
  float nMuonsPlus, nMuonsMinus;
  float muon1Pt, muon2Pt, muon3Pt, muon4Pt;
  float muon1Eta, muon2Eta, muon3Eta, muon4Eta;
  float muon1Phi, muon2Phi, muon3Phi, muon4Phi;
  float muon1Dxy, muon2Dxy, muon3Dxy, muon4Dxy;
  float muon1Dz, muon2Dz, muon3Dz, muon4Dz;
  float muon1SIP, muon2SIP, muon3SIP, muon4SIP;
  float muon1Isol, muon2Isol, muon3Isol, muon4Isol;
  float muon1ID, muon2ID, muon3ID, muon4ID;
  } ENTRY4Mu;

typedef struct {                                                                                                                                                                                   
      
  ///Variables saved into a TTree                                                                                                                                                      
  float nMuons, mass2Mu;       
  float MET, mT;                                                                                                                                                                                  
  float muonPt, muonEta, muonPhi;
  float muonSIP, muonDxy, muonDz, muonIsol, muonID;
  float nBJets30, nLightJets30;
  float mass2Jets,  mass3Jets;
} ENTRY1Mu;


class HZZAnalyzer: public Analyzer{

 public:

  HZZAnalyzer(const std::string & aName, const std::string & aDecayMode = "None");

  virtual ~HZZAnalyzer();

  ///Initialize the analyzer
  virtual void initialize(TDirectory* aDir,
			  pat::strbitset *aSelections);

  virtual bool analyze(const EventProxyBase& iEvent);

  virtual bool analyze(const EventProxyBase& iEvent, ObjectMessenger *aMessenger){return analyze(iEvent); }

  virtual void finalize();

  virtual void clear(){;};

  virtual void addBranch(TTree *);

  Analyzer* clone() const;

  bool filter() const{ return filterEvent_;};


 protected:

  pat::strbitset *mySelections_;

  ///Types of the selection flow
  std::vector<std::string> selectionFlavours_;

 private:

  void getPreselectionEff(const EventProxyHTT & myEventProxy);

  void getSeparatedJets(const EventProxyHTT & myEventProxy, float deltaR);

  bool getGoodMuons(const EventProxyHTT & myEventProxy);

  void setHistos(HZZHistograms *histos) { myHistos_ = histos;};

  bool fill4MuonTree();

  bool fill1MuonTree();

  ///Histograms storage.
  HZZHistograms *myHistos_;

  //should this HZZAnalyzer be able to filter events
  bool filterEvent_;

  ///Reconstructed objects selected for given event.
  HTTEvent aEvent;

  ///Pointer to the ROOT file containing current TTree
  TFile *ntupleFile_;

  TH1F *hStatsFromFile_;


  bool applyCuts = false;
  unsigned int muonIDmask = (1<<6);
  std::vector<const HTTParticle *> mySeparatedJets;;
  std::vector<const HTTParticle *> myMuonsPlus; 
  std::vector<const HTTParticle *> myMuonsMinus; 
  std::vector<const HTTParticle *> myMuons;
  TLorentzVector myMET;

  ENTRY4Mu aEntry4Mu;
  ENTRY1Mu aEntry1Mu;


};

#endif
