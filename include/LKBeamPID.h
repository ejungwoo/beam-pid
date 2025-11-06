#ifndef LKBEAMPID_HH
#define LKBEAMPID_HH

#include "LKParameterContainer.h"
#include "LKDrawingGroup.h"
#include "LKDrawing.h"
#include "LKPainter.h"
#include "LKBinning.h"
#include "LKLogger.h"

#include "TObjArray.h"
#include "TString.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF2.h"
#include "TF1.h"

#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

class LKBeamPID
{
    public:
        LKBeamPID();

        bool ListFiles(TString path="./", TString format="gen.root");
        bool SelectFile(int idx=-1);
        void CreateAndFillHistogram(int printb=1);
        void DrawPoints(vector<vector<double>> points=vector<vector<double>>{});
        void DrawPoints(int redraw=0, vector<vector<double>> points=vector<vector<double>>{});
        void FitTotal();
        void MakeSummary();

        LKDrawing* GetFitTestDrawing(int i, TH2D *hist, TF2* fit, TF2* fitContanminent=(TF2*)nullptr);
        TF2* Fit2DGaussian(TH2D *hist, int idx, double valueX, double valueY, double sigmaX=2.5, double sigmaY=1.5, double theta=21);
        TGraph *GetContourGraph(double contoA, double amplit, double valueX, double sigmaX, double valueY, double sigmaY, double thetaR);
        double IntegralInsideGraph(TH2D* hist, TGraph* graph, bool justCount=true);
        double IntegralInsideGraph(TH2D* hist, TGraph* graph, TF2 *f2, bool justCount=true);
        double Integral2DGaussian(double amplitude, double sigma_x, double sigma_y, double contoA=0);
        double Integral2DGaussian(TF2 *f2, double contoA=0);
        void CollectRootFiles(std::vector<TString> &listGenFile, const char *dataPath="data", const TString &format="gen.root");

        void PrintBinning();

        void Help(TString mode="help");
        void ResetBinning();
        void SaveBinning();
        void SetSValue(double scale=-1);
        void SetXBinSize(double w, int fill=1);
        void SetYBinSize(double w, int fill=1);
        void SetGausFitRange(double sigDist=-1);
        void SetRunNumber(int run);
        void SaveConfiguration();

        void SetBinningX(int nx, double x1, double x2, int fill=0);
        void SetBinningY(int ny, double y1, double y2, int fill=0);
        void SetRangeX(double x1, double x2, int fill=0);
        void SetRangeY(double y1, double y2, int fill=0);
        void SetBinning(int nx, double x1, double x2, int ny, double y1, double y2);
        void SetBinning(double x1, double x2, double y1, double y2);
        void SetBinNX(double n, int fill=0);
        void SetBinNY(double n, int fill=0);

    private:
        int fStage = 0;
        bool fUse2D = true;
        bool fUse3D = false;
        bool fUseLogz = false;
        LKDrawingGroup *fTop = nullptr, *fGroupFit = nullptr, *fGroupPID = nullptr;
        LKDrawing *fDraw2D = nullptr, *fDraw3D = nullptr;
        TObjArray *fHistDataArray = nullptr, *fHistFitGArray = nullptr, *fHistBackArray = nullptr, *fHistTestArray = nullptr, *fHistErrrArray = nullptr;
        TObjArray* fFitArray = nullptr;
        TObjArray* fContourGraphArray = nullptr;
        vector<TString> fListGenFile;
        TFile* fDataFile = nullptr;
        TTree* fDataTree = nullptr;
        TH2D *fHistPID = nullptr;
        bool fRunCollected = false, fInitialized = false;
        int fCurrentRunNumber=999999999, fCurrentType;
        double fSigDist = 1;
        int fNumContours = 10;
        LKBinning fBnn2, fBnn3;
        TString fCurrentFileName;
        double fFinalContourAScale = 0.2;
        TGraph* fFinalContourGraph = nullptr;
        vector<double> fContourScaleList = {0.9,0.5};
        const TString fFormulaRotated2DGaussian = "[0]*exp(-0.5*(pow(((x-[1])*cos([5])+(y-[3])*sin([5]))/[2],2)+pow((-(x-[1])*sin([5])+(y-[3])*cos([5]))/[4],2)))";

    ClassDef(LKBeamPID, 1)
};

//#define c1 Help();
//#define c2 PrintBinning();
//#define c3 ResetBinning();
//#define c4 SaveBinning();
//#define c5 SetXBinSize();
//#define c6 SetYBinSize();
//#define c7 SetSValue();
//#define c8 SetGausFitRange();
//#define c9 SaveConfiguration();
//
//#define a1 ListFiles();
//#define a2 SelectFile();
//#define a3 ClickSelectPoints();
//#define a4 FitTotal();
//#define a5 MakeSummary();
//#define b1 RedrawPlot();
//#define b2 RedrawAndSelectPoints();

#define e_title std::cout << "\033[0;35m" << "[RUN-" << fCurrentRunNumber << "] ============= "  << "\033[0m"

#endif
