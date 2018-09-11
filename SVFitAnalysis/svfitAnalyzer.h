
#ifndef RootAnalysis_svfitAnalyzer_H
#define RootAnalysis_svfitAnalyzer_H

#include <string>
#include <vector>
#include <map>
#include <tuple>

#include "ObjectMessenger.h"
#include "EventProxyBase.h"
#include "EventProxyHTT.h"

#include "strbitset.h"
#include "TDirectory.h"

//ROOT includes
#include "TTree.h"
#include "TList.h"
#include "TRandom3.h"

#include "Analyzer.h"
#include "ChannelSpecifics.h"
#include "AnalysisEnums.h"

#include "TauAnalysis/ClassicSVfit/interface/ClassicSVfit.h"
#include "TauAnalysis/ClassicSVfit/interface/FastMTT.h"

class TH1F;
class TH2F;
class TH3F;
class TLorentzVector;
class TF1;

class svfitAnalyzer: public Analyzer{

  friend class ChannelSpecifics;
  friend class MuTauSpecifics;
  friend class TauTauSpecifics;
  friend class MuMuSpecifics;

 public:

  svfitAnalyzer(const std::string & aName, const std::string & aDecayMode = "None");

  virtual ~svfitAnalyzer();

  ///Initialize the analyzer
  virtual void initialize(TDirectory* aDir,
			  pat::strbitset *aSelections);

  virtual bool analyze(const EventProxyBase& iEvent, ObjectMessenger *aMessenger);

  virtual bool analyze(const EventProxyBase& iEvent){return analyze(iEvent, nullptr); }

  virtual void finalize();

  virtual void clear(){;};

  virtual void addBranch(TTree *);

  Analyzer* clone() const;

  bool filter() const{ return filterEvent_;};

  void setAnalysisObjects(const EventProxyHTT & myEventProxy);

  ///Check it the event passes given category selections.
  bool passCategory(unsigned int iCategory);

  ///Return human readable sample name (Data, WJets, etc).
  std::string getSampleName(const EventProxyHTT & myEventProxy);

  ///Return human readable sample name (Data, WJets, etc).
  ///Make the methos static, so other modules can use it.
  ///Method used when sample coding in TTree is not present.
  ///In this case a ROOT file name is used to decode the sample type.
  std::string getSampleNameFromFileName(const EventProxyHTT & myEventProxy);

  ///Return sample name for DY. Name encoded jet bin, and decay mode.
  std::string getDYSampleName(const EventProxyHTT & myEventProxy);

  //Return name sample name suffix for different particles matched to reconstructed tau
  std::string getMatchingName(const EventProxyHTT & myEventProxy);

  ///Get jets separated by deltaR from tau an muon.
  std::vector<HTTParticle> getSeparatedJets(const EventProxyHTT & myEventProxy,
    float deltaR);

 protected:

  pat::strbitset *mySelections_;

  ///Types of the selection flow
  std::vector<std::string> selectionFlavours_;

 private:

  TLorentzVector computeMTT(const std::string & algoName);
  
  TLorentzVector runSVFitAlgo(const std::vector<classic_svFit::MeasuredTauLepton> & measuredTauLeptons,
                              const TVector2 &aMET, const TMatrixD &covMET);

  TLorentzVector runFastMTTAlgo(const std::vector<classic_svFit::MeasuredTauLepton> & measuredTauLeptons,
				const TVector2 &aMET, const TMatrixD &covMET);

  std::tuple<double, double> getTauMomentum(const TLorentzVector & visP4, double cosGJ);

  ///Parts of code specific to give decay channel.
  ///In particular category and object selection.
  ChannelSpecifics *myChannelSpecifics;

  //should this HTTAnalyzer be able to filter events
  bool filterEvent_;

  ///Map from file name to sample name.
  std::map<std::string, std::string> fileName2sampleName;

  ///Reconstructed objects selected for given event.
  HTTEvent aEvent;
  HTTPair aPair;
  std::string sampleName;

  HTTParticle aLeg2, aLeg1, aMET;
  HTTParticle aGenLeg1, aGenLeg2;
  float aGenSumM;
  float higgs_mass_trans;
  HTTParticle aJet1, aJet2, aBJet1;
	TMatrixD _covMET;
  std::vector<HTTParticle> aSeparatedJets;
  int nJets30;
  int nJetsInGap30;

  std::vector<bool> categoryDecisions;
  unsigned int myNumberOfCategories;

  ClassicSVfit svFitAlgo;
  FastMTT fastMTTAlgo;

  TF1 *fLikelihood;

  TRandom3 aRndm;
  ///UGLY
  TLorentzVector svFitLeg1P4, svFitLeg2P4;
  /////////

};

#endif
