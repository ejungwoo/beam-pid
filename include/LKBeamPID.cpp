#include "LKBeamPID.h"

ClassImp(LKBeamPID)

LKBeamPID::LKBeamPID()
{
    fStage = 0;
    gStyle -> SetNumberContours(100);
    gStyle -> SetOptStat("e");
    ResetBinning();
    fFitArray = new TObjArray();
    fHistDataArray = new TObjArray();
    fHistFitGArray = new TObjArray();
    fHistBackArray = new TObjArray();
    fHistTestArray = new TObjArray();
    fHistErrrArray = new TObjArray();
    fTop = new LKDrawingGroup();
    fGroupFit = fTop -> CreateGroup(Form("event_fit_%04d",fCurrentRunNumber));
    fGroupPID = fTop -> CreateGroup(Form("event_pid_%04d",fCurrentRunNumber));
    fDraw2D = fGroupPID -> CreateDrawing(Form("draw_2d_%04d",fCurrentRunNumber));
    fDraw2D -> SetCanvasSize(1,1,1);
    fDraw2D -> SetCanvasMargin(.11,.15,.11,0.08);
    if (fUseLogz) fDraw2D -> SetLogz();
}

void LKBeamPID::Help(TString mode)
{
    if (mode.Index("help")>=0) {
        PrintBinning();
        e_cout << "== List of main functions" << endl;
        e_cout << "   - ListFiles(TString path, TString format)" << endl;
        e_cout << "   - SelectFile(int idx)" << endl;
        e_cout << "   - UseCurrentgPad()" << endl;
        e_cout << "   - SelectCenters()" << endl;
        e_cout << "   - Redraw()" << endl;
        e_cout << "   - RelectCenters()" << endl;
        e_cout << "   - FitTotal()" << endl;
        e_cout << "   - MakeSummary()" << endl;
        e_cout << endl;
        e_cout << "== List of optional functions" << endl;
        e_cout << "   - Help(TString mode)" << endl;
        e_cout << "   - ResetBinning()" << endl;
        e_cout << "   - SaveBinning()" << endl;
        e_cout << "   - SetSValue(double scale)" << endl;
        e_cout << "   - SetXBinSize(double w)" << endl;
        e_cout << "   - SetYBinSize(double w)" << endl;
        e_cout << "   - SetGausFitRange(double sigDist)" << endl;
        e_cout << "   - SetRunNumber(int run)" << endl;
        e_cout << "   - SaveConfiguration()" << endl;
    }

    //if (mode.Index("i")>=0) {
    //    e_note << "List of binning methods:" << endl;
    //      e_cout << "   SetRangeX(x1, x2)" << endl;
    //      e_cout << "   SetRangeY(y1, y2)" << endl;
    //      e_cout << "   SetBinningX(nx, x1, x2)" << endl;
    //      e_cout << "   SetBinningY(ny, y1, y2)" << endl;
    //      e_cout << "   SetBinning(x1, x2, y1, y2)" << endl;
    //      e_cout << "   SetBinning(nx, x1, x2, ny, y1, y2)" << endl;
    //      e_cout << "   SetBinWX(w)" << endl;
    //      e_cout << "   SetBinWY(w)" << endl;
    //      e_cout << "   SetBinNX(n)" << endl;
    //      e_cout << "   SetBinNY(n)" << endl;
    //      e_cout << "   SaveBinning()" << endl;
    //}
    //else                    e_info << "Enter 'ii' to print list of methods for binning setting" << endl;
    //                        e_info << "Enter 'bb' to reset binning configuration" << endl;
    //                        e_info << "Enter 'vv' to save current binning configuration" << endl;
    //                        e_info << "Enter 'ee' to change the final contour amplitude scale (" << fFinalContourAScale << ")" << endl;
    //if (mode.Index("a")>=0) e_note << "Enter 'aa' to print list of files" << endl;
    //if (mode.Index("s")>=0) e_note << "Enter 'ss' to select files" << endl;
    //if (mode.Index("d")>=0) e_note << "Enter 'dd' to draw and select PID centers" << endl;
    //if (mode.Index("t")>=0) e_note << "Enter 'tt' to redraw" << endl;
    //if (mode.Index("r")>=0) e_note << "Enter 'rr' to redraw and select PID centers" << endl;
    //if (mode.Index("f")>=0) e_note << "Enter 'ff' to fit histogram simultaneously using previous fits" << endl;
    //if (mode.Index("g")>=0) e_note << "Enter 'gg' to make summary of the result" << endl;
    //if (mode.Index("q")>=0) e_info << "Enter 'qq' to change fit range" << endl;
}

bool LKBeamPID::ListFiles(TString path, TString format)
{
    e_title << "ListFiles" << endl;
    if (!fRunCollected) {
        CollectRootFiles(fListGenFile,path,format);
        fRunCollected = true;
    }

    if (fListGenFile.size()==0) {
        e_info << "End of file list" << endl;
        return false;
    }

    int count = 0;
    e_cout << endl;
    e_info << "List of files" << endl;
    for (auto fileName : fListGenFile) {
        if (count%4==0) e_cout << "--------" << endl;
        e_cout << "   " << left << setw(5) << count++ << " " << fileName << endl;
    }
    e_cout << "--------" << endl;
    e_cout << endl;
    Help("s");

    fStage = 1;
    return true;
}

bool LKBeamPID::SelectFile(int index)
{
    e_title << "SelectFile" << endl;
    while (true)
    {
        if (index<0) {
            e_note << "Enter index number from the file list (q:exit): ";
            TString inputString;
            cin >> inputString;
            inputString = inputString.Strip(TString::kBoth);
            if (inputString=="x"||inputString=="q") { e_cout << "quit" << endl; return false; }
            if (inputString.IsDigit()==false) {
                e_info << "Invalid input." << endl;
                continue;
            }
            index = inputString.Atoi();
        }
        if (index<0||index>=fListGenFile.size()) {
            e_warning << "Index not in range. (size=" << fListGenFile.size() << ")" << endl;
            return false;
        }
        fCurrentFileName = fListGenFile.at(index);
        TString runNumberString = TString(fCurrentFileName(fCurrentFileName.Index("chkf")+8,4));
        if (runNumberString.IsDigit()) fCurrentRunNumber = runNumberString.Atoi();
        else fCurrentRunNumber=999999999;
        fListGenFile.erase(fListGenFile.begin()+index);
        e_cout << "   " << fCurrentFileName << endl;
        e_cout << "   (this file will be removed from the list)" << endl;
        break;
    }

    if (fDataFile) fDataFile -> Close();
    fDataFile = new TFile(fCurrentFileName,"read");
    fDataTree = (TTree*) fDataFile -> Get("tree");
    if      (fCurrentFileName.Index("chkf2run")>=0) { fCurrentType = 2; fYName = "f2ssde"; }
    else if (fCurrentFileName.Index("chkf3run")>=0) { fCurrentType = 3; fYName = "f3ssde"; }
    fXName = "rf0";

    if (!fInitialized)
    {
        fInitialized = true;
    }
    fStage = 2;
    CreateAndFillHistogram(0);

    fGroupFit -> SetName(Form("event_fit_%04d",fCurrentRunNumber));
    fGroupPID -> SetName(Form("event_pid_%04d",fCurrentRunNumber));
    fDraw2D -> SetName(Form("draw_2d_%04d",fCurrentRunNumber));
    fDraw2D -> Draw();
    e_cout << endl;
    Help("td");

    return true;
}

void LKBeamPID::CreateAndFillHistogram(int printb)
{
    if (printb) {
        e_cout << "   " << fBnn2.Print(false) << endl;
        e_cout << "   " << fBnn3.Print(false) << endl;
    }
    if (fDataTree) {
        fHistPID = fBnn2.NewH2(Form("histPID_%04d",fCurrentRunNumber),Form(";%s;%s",fXName.Data(),fYName.Data()));
        Long64_t entriesTotal = fDataTree->Draw(Form("%s:%s>>%s",fYName.Data(),fXName.Data(),fHistPID->GetName()),"","goff");
        e_info << "Total entries =" << entriesTotal << " (hist)=" << fHistPID -> GetEntries() << endl;
        fHistPID -> GetXaxis() -> SetTitleOffset(1.2);
        fHistPID -> SetTitle(Form("RUN %04d",fCurrentRunNumber));
        fDraw2D -> Clear();
        fDraw2D -> Add(fHistPID, "colz");
        fDraw2D -> SetStatsFillStyle(3001);
        fDraw2D -> Print(); // XXX
        fStage = 3;
    }
}

void LKBeamPID::UsePad(TVirtualPad *pad)
{
    fDraw2D -> Clear();
    fDraw2D -> Add(gPad);
    fDraw2D -> Clear("!main:!cvs");
    fHistPID = (TH2D*) fDraw2D -> GetMainHist();
    fXName = fHistPID -> GetXaxis() -> GetTitle();
    fYName = fHistPID -> GetYaxis() -> GetTitle();
    fCurrentType = 1;
    fStage = 4;
    fDraw2D -> Draw();
    return;
}

void LKBeamPID::ReselectCenters()
{
    if (fStage<2) {
        e_warning << "Must select file before drawing points!" << endl;
        return;
    }
    fDraw2D -> Clear("!main:!cvs");
    fDraw2D -> Draw();
    SelectCenters();
}

void LKBeamPID::Redraw()
{
    if (fStage<2) {
        e_warning << "Must select file before drawing points!" << endl;
        return;
    }
    fDraw2D -> Clear("!main:!cvs");
    fDraw2D -> Draw();
    Help("tr");
}

void LKBeamPID::SelectCenters(vector<vector<double>> points)
{
    e_title << "SelectCenters" << endl;
    if (fStage<2) {
        e_warning << "Must select file before drawing points!" << endl;
        return;
    }

    if (points.size()==0)
    {
        e_cout << endl;
        e_info << "Draw graph with the mouse pointer! Double click to end." << endl;
        auto graph2 = (TGraph*) fDraw2D -> GetCanvas() -> WaitPrimitive("Graph","PolyLine");
        auto graph = (TGraph*) graph2 -> Clone();
        graph -> SetMarkerStyle(20);
        fDraw2D -> Add(graph,"samep");
        auto numPoints = graph -> GetN();
        for (auto iPoint=0; iPoint<numPoints; ++iPoint)
        {
            double x = graph -> GetPointX(iPoint);
            double y = graph -> GetPointY(iPoint);
            e_cout << "   " << iPoint << " " << x << " " << y << endl;
            points.push_back(vector<double>{x,y});
        }
    }

    fGroupFit -> Clear();
    double wx = fHistPID -> GetXaxis() -> GetBinWidth(1);
    double wy = fHistPID -> GetYaxis() -> GetBinWidth(1);
    double binA = wx*wy;

    e_info << "Total " << points.size() << " pid centers are selected" << endl;

    int count = 0;
    fFitArray -> Clear();
    for (auto point : points)
    {
        auto fit = Fit2DGaussian(fHistPID, count, point[0], point[1]);
        auto idx = fFitArray -> GetEntries();
        fFitArray -> Add(fit);
        if (fUse2D) {
            double xx1, xx2, yy1, yy2;
            fit -> GetParLimits(1,xx1,xx2);
            fit -> GetParLimits(3,yy1,yy2);
            auto graphCenterRange = new TGraph();
            graphCenterRange -> SetName("graphCenterRange");
            graphCenterRange -> SetLineColor(kGreen);
            graphCenterRange -> SetLineStyle(9);
            graphCenterRange -> SetPoint(0,xx1,yy1);
            graphCenterRange -> SetPoint(1,xx2,yy1);
            graphCenterRange -> SetPoint(2,xx2,yy2);
            graphCenterRange -> SetPoint(3,xx1,yy2);
            graphCenterRange -> SetPoint(4,xx1,yy1);
            fit -> GetRange(xx1,yy1,xx2,yy2);
            auto graphFitRange = new TGraph();
            graphFitRange -> SetName("graphFitRange");
            graphFitRange -> SetLineColor(kYellow);
            graphFitRange -> SetLineStyle(9);
            graphFitRange -> SetPoint(0,xx1,yy1);
            graphFitRange -> SetPoint(1,xx2,yy1);
            graphFitRange -> SetPoint(2,xx2,yy2);
            graphFitRange -> SetPoint(3,xx1,yy2);
            graphFitRange -> SetPoint(4,xx1,yy1);
            fit -> SetRange(xx1-(xx2-xx1),yy1-(yy2-yy1),xx2+(xx2-xx1),yy2+(yy2-yy1));
            fDraw2D -> Add(graphCenterRange,"samel");
            fDraw2D -> Add(graphFitRange,"samel");
        }
        if (fUse2D)
        {
            auto amplit = fit->GetParameter(0);
            auto valueX = fit->GetParameter(1);
            auto sigmaX = fit->GetParameter(2);
            auto valueY = fit->GetParameter(3);
            auto sigmaY = fit->GetParameter(4);
            auto thetaR = fit->GetParameter(5);
            for (double contourScale : fContourScaleList) {
                auto graphC = GetContourGraph(contourScale*amplit, amplit, valueX, sigmaX, valueY, sigmaY, thetaR);
                graphC -> SetName(Form("graphC_%d_S%s",idx,LKMisc::RemoveTrailing0(100*contourScale,1).Data()));
                graphC -> SetLineColor(kRed);
                graphC -> SetLineStyle(9);
                fDraw2D -> Add(graphC,"samel");
            }
            auto graphC = GetContourGraph(fFinalContourAScale*amplit, amplit, valueX, sigmaX, valueY, sigmaY, thetaR);
            graphC -> SetName(Form("graphC_%d_S%s",idx,LKMisc::RemoveTrailing0(100*fFinalContourAScale,1).Data()));
            graphC -> SetLineColor(kRed);
            fDraw2D -> Add(graphC,"samel");
        }
        count++;
    }
    //fDraw2D -> Print("all");
    //fDraw2D -> Draw("debug_draw");
    fGroupFit -> Draw();
    fDraw2D -> Draw();

    Help("rqf");

    fStage = 5;
    return;
}

void LKBeamPID::FitTotal()
{
    e_title << "FitTotal" << endl;
    if (fStage<2) {
        e_warning << "Must select file before fit total pid!" << endl;
        return;
    }
    else if (fStage<5) {
        e_warning << "Must select points before fit total pid!" << endl;
        return;
    }

    fDraw2D -> Clear("!main:!cvs");
    fGroupFit -> Clear();
    auto numFits = fFitArray -> GetEntries();
    TString formulaTotal;
    double xx1=DBL_MAX, xx2=-DBL_MAX, yy1=DBL_MAX, yy2=-DBL_MAX;
    for (auto iFit=0; iFit<numFits; ++iFit)
    {
        TString formulaCurrent = fFormulaRotated2DGaussian.Data();
        formulaCurrent.ReplaceAll("[0]",Form("[%d]",0+iFit*6));
        formulaCurrent.ReplaceAll("[1]",Form("[%d]",1+iFit*6));
        formulaCurrent.ReplaceAll("[2]",Form("[%d]",2+iFit*6));
        formulaCurrent.ReplaceAll("[3]",Form("[%d]",3+iFit*6));
        formulaCurrent.ReplaceAll("[4]",Form("[%d]",4+iFit*6));
        formulaCurrent.ReplaceAll("[5]",Form("[%d]",5+iFit*6));
        if (iFit==0) formulaTotal = formulaCurrent;
        else formulaTotal = formulaTotal + " + " + formulaCurrent;
        auto fit = (TF2*) fFitArray -> At(iFit);
        auto amplit = fit->GetParameter(0);
        auto valueX = fit->GetParameter(1);
        auto sigmaX = fit->GetParameter(2);
        auto valueY = fit->GetParameter(3);
        auto sigmaY = fit->GetParameter(4);
        auto thetaR = fit->GetParameter(5);
        auto x1 = valueX-fSigDist*sigmaX; if (xx1>x1) xx1 = x1;
        auto x2 = valueX+fSigDist*sigmaX; if (xx2<x2) xx2 = x2;
        auto y1 = valueY-fSigDist*sigmaY; if (yy1>y1) yy1 = y1;
        auto y2 = valueY+fSigDist*sigmaY; if (yy2<y2) yy2 = y2;
    }
    auto fitContanminent = new TF2(Form("fitContanminent_%04d", fCurrentRunNumber), formulaTotal, xx1, xx2, yy1, yy2);
    auto fitTotal = new TF2(Form("fitTotal_%04d", fCurrentRunNumber), formulaTotal, xx1, xx2, yy1, yy2);
    fitTotal -> SetLineColor(kMagenta);
    fitTotal -> SetContour(3);
    for (auto iFit=0; iFit<numFits; ++iFit)
    {
        auto fit = (TF2*) fFitArray -> At(iFit);
        auto amplit = fit->GetParameter(0);
        auto valueX = fit->GetParameter(1);
        auto valueY = fit->GetParameter(2);
        auto sigmaX = fit->GetParameter(3);
        auto sigmaY = fit->GetParameter(4);
        auto thetaR = fit->GetParameter(5);
        fitTotal -> SetParameter(0+iFit*6, amplit);
        fitTotal -> SetParameter(1+iFit*6, valueX);
        fitTotal -> SetParameter(2+iFit*6, valueY);
        fitTotal -> SetParameter(3+iFit*6, sigmaX);
        fitTotal -> SetParameter(4+iFit*6, sigmaY);
        fitTotal -> SetParameter(5+iFit*6, thetaR);
        fitTotal -> SetParLimits(0+iFit*6, amplit*0.8, amplit*1.2);
        fitTotal -> SetParLimits(1+iFit*6, valueX-0.1*sigmaX, valueX+0.1*sigmaX);
        fitTotal -> SetParLimits(2+iFit*6, valueY-0.1*sigmaY, valueY+0.1*sigmaY);
        fitTotal -> SetParLimits(3+iFit*6, sigmaX*0.8, sigmaX*1.2);
        fitTotal -> SetParLimits(4+iFit*6, sigmaY*0.8, sigmaY*1.2);
        fitTotal -> SetParLimits(5+iFit*6, thetaR-0.1*TMath::Pi(), thetaR+0.1*TMath::Pi());
    }
    e_info << "Fitting " << numFits << " PIDs in " << Form("x=(%f,%f), y=(%f,%f) ...",xx1,xx2,yy1,yy2) << endl;
    fHistPID -> Fit(fitTotal,"QBR0");
    auto legend = new TLegend();
    legend -> SetFillStyle(3001);
    legend -> SetMargin(0.1);
    for (auto iFit=0; iFit<numFits; ++iFit)
    {
        auto fit = (TF2*) fFitArray -> At(iFit);
        auto amplit = fitTotal->GetParameter(0+iFit*6);
        auto valueX = fitTotal->GetParameter(1+iFit*6);
        auto sigmaX = fitTotal->GetParameter(2+iFit*6);
        auto valueY = fitTotal->GetParameter(3+iFit*6);
        auto sigmaY = fitTotal->GetParameter(4+iFit*6);
        auto thetaR = fitTotal->GetParameter(5+iFit*6);
        fit -> SetParameter(0,amplit);
        fit -> SetParameter(1,valueX);
        fit -> SetParameter(2,sigmaX);
        fit -> SetParameter(3,valueY);
        fit -> SetParameter(4,sigmaY);
        fit -> SetParameter(5,thetaR);
        fit -> SetLineColor(kMagenta);
        e_cout << "   " << fit -> GetName() << ": " << std::left
            << setw(16) << Form("amp=%f,", amplit)
            << setw(28) << Form("x=(%f, %f),", valueX, sigmaX)
            << setw(28) << Form("y=(%f, %f),", valueY, sigmaY)
            << setw(18) << Form("theta=%f", thetaR*TMath::RadToDeg())
            << endl;
        for (double contourScale : fContourScaleList) {
            auto graphC = GetContourGraph(contourScale*amplit, amplit, valueX, sigmaX, valueY, sigmaY, thetaR);
            graphC -> SetLineColor(kRed);
            graphC -> SetLineStyle(9);
            fDraw2D -> Add(graphC,"samel");
        }
        auto graphC = GetContourGraph(fFinalContourAScale*amplit, amplit, valueX, sigmaX, valueY, sigmaY, thetaR);
        graphC -> SetLineColor(kRed);
        fDraw2D -> Add(graphC,"samel");
        auto text = new TText(valueX,valueY,Form("%d",iFit));
        text -> SetTextAlign(22);
        text -> SetTextSize(0.02);
        if (amplit<fHistPID->GetMaximum()*0.3)
            text -> SetTextColor(kGreen);
        fDraw2D -> Add(text,"same");
        for (auto iPar=0; iPar<fitTotal->GetNpar(); ++iPar)
            fitContanminent->SetParameter(iPar,fitTotal->GetParameter(iPar));
        fitContanminent->SetParameter(0+iFit*6,0);
        auto draw = GetFitTestDrawing(iFit,fHistPID,fit,fitContanminent);
        fGroupFit -> Add(draw);
        TH1D *histData = (TH1D*) fHistDataArray -> At(iFit);
        TH1D *histBack = (TH1D*) fHistBackArray -> At(iFit);
        auto bin = histData -> FindBin(fFinalContourAScale+0.5*(1./fNumContours));
        auto count = histData -> GetBinContent(bin);
        auto contamination = histBack -> GetBinContent(bin);
        auto corrected = count - contamination;
        //fDraw2D -> AddLegendLine(Form("%d) cc=%d",iFit,int(corrected)));
        legend -> AddEntry((TObject*)nullptr,Form("[%d] %d (%d)",iFit,int(count),int(contamination)),"");
    }
    if (fUse2D) {
        auto graphFitRange = new TGraph();
        graphFitRange -> SetLineColor(kYellow);
        graphFitRange -> SetLineStyle(2);
        graphFitRange -> SetPoint(0,xx1,yy1);
        graphFitRange -> SetPoint(1,xx2,yy1);
        graphFitRange -> SetPoint(2,xx2,yy2);
        graphFitRange -> SetPoint(3,xx1,yy2);
        graphFitRange -> SetPoint(4,xx1,yy1);
        fDraw2D -> Add(graphFitRange,"samel");
    }
    //fDraw2D -> SetCreateLegend();
    fDraw2D -> SetLegendCorner(1);
    fDraw2D -> Add(legend);
    fGroupFit -> Draw();
    fDraw2D -> Draw();

    Help("rqg");
    fStage = 6;
}

void LKBeamPID::MakeSummary()
{
    e_title << "MakeSummary" << endl;
    if (fStage<2) {
        e_warning << "Must select file before creating summary!" << endl;
        return;
    }
    else if (fStage<5) {
        e_warning << "Must select points before creating summary!" << endl;
        return;
    }
    gSystem -> Exec("mkdir -p summary/");
    TString summaryName1 = Form("summary/run_%04d.root",fCurrentRunNumber);
    TString summaryName2 = Form("summary/run_%04d.txt",fCurrentRunNumber);
    e_info << "Summary files " << endl;
    for (TString summaryName : {summaryName1,summaryName2}) e_cout << "   " << summaryName << endl;
    for (TString summaryName : {summaryName2})
    {
        ofstream fileSummary(summaryName);
        auto numFits = fFitArray -> GetEntries();
        for (auto iFit=0; iFit<numFits; ++iFit)
        {
            auto total = fHistPID -> GetEntries();
            TH1D *histData = (TH1D*) fHistDataArray -> At(iFit);
            TH1D *histBack = (TH1D*) fHistBackArray -> At(iFit);
            auto bin = histData -> FindBin(fFinalContourAScale+0.5*(1./fNumContours));
            auto count = histData -> GetBinContent(bin);
            auto contamination = histBack -> GetBinContent(bin);
            auto corrected = count - contamination;
            auto fit = (TF2*) fFitArray -> At(iFit);
            auto amplit = fit->GetParameter(0);
            auto valueX = fit->GetParameter(1);
            auto sigmaX = fit->GetParameter(2);
            auto valueY = fit->GetParameter(3);
            auto sigmaY = fit->GetParameter(4);
            auto thetaR = fit->GetParameter(5);
            fileSummary << "####################################################" << endl;
            fileSummary << left;
            fileSummary << setw(25) << Form("pid%d/total",iFit)              << total << endl;
            fileSummary << setw(25) << Form("pid%d/ca_scale",iFit)           << fFinalContourAScale << endl;
            fileSummary << setw(25) << Form("pid%d/count",iFit)              << count << endl;
            fileSummary << setw(25) << Form("pid%d/contamination",iFit)      << contamination << endl;
            fileSummary << setw(25) << Form("pid%d/corrected",iFit)          << corrected << endl;
            fileSummary << setw(25) << Form("pid%d/purity",iFit)             << corrected/total << endl;
            fileSummary << setw(25) << Form("pid%d/amplitude",iFit)          << amplit << endl;
            fileSummary << setw(25) << Form("pid%d/x(x_rf0)",iFit)           << valueX << endl;
            fileSummary << setw(25) << Form("pid%d/y(%s)",iFit,fYName.Data()) << valueY << endl;
            fileSummary << setw(25) << Form("pid%d/sigma_x",iFit)            << sigmaX << endl;
            fileSummary << setw(25) << Form("pid%d/sigma_y",iFit)            << sigmaY << endl;
            fileSummary << setw(25) << Form("pid%d/theta_deg",iFit)          << thetaR*TMath::RadToDeg() << endl;
            if (fFinalContourGraph!=nullptr) {
                auto numPoints = fFinalContourGraph -> GetN();
                fileSummary << setw(25) << Form("pid%d/contour_x  ",iFit);
                for (auto iPoint=0; iPoint<numPoints; ++iPoint) fileSummary << fFinalContourGraph -> GetPointX(iPoint) << ","; fileSummary << endl;
                fileSummary << setw(25) << Form("pid%d/contour_y  ",iFit);
                for (auto iPoint=0; iPoint<numPoints; ++iPoint) fileSummary << fFinalContourGraph -> GetPointY(iPoint) << ","; fileSummary << endl;
            }
            fileSummary << endl;
        }
    }

    {
        auto fileSummary1 = new TFile(summaryName1,"recreate");

        auto cvsPID = fDraw2D -> GetCanvas();
        auto cvsFit = fGroupFit -> GetCanvas();
        if (cvsPID!=nullptr||cvsFit!=nullptr) gSystem -> Exec("mkdir -p figures/");
        if (cvsPID!=nullptr) {
            cvsPID -> SaveAs(Form("figures/pid_%04d.png",fCurrentRunNumber));
            fileSummary1 -> cd();
            cvsPID -> Write("pid");
        }
        if (cvsFit!=nullptr) {
            cvsFit -> SaveAs(Form("figures/fit_%04d.png",fCurrentRunNumber));
            cvsFit -> Write("fit");
        }

        fTop -> Write("flat");
    }

    Help("ra");
    fStage = 7;
}

LKDrawing* LKBeamPID::GetFitTestDrawing(int idx, TH2D *hist, TF2* fit, TF2* fitContanminent)
{
    gStyle->SetPaintTextFormat(".3f");
    auto amplit = fit->GetParameter(0);
    auto valueX = fit->GetParameter(1);
    auto sigmaX = fit->GetParameter(2);
    auto valueY = fit->GetParameter(3);
    auto sigmaY = fit->GetParameter(4);
    auto thetaR = fit->GetParameter(5);
    TString nameData = fit -> GetName(); nameData .ReplaceAll("fit_","histIntegralData_");
    TString nameFitG = fit -> GetName(); nameFitG .ReplaceAll("fit_","histIntegralFitG_");
    TString nameTest = fit -> GetName(); nameTest .ReplaceAll("fit_","histIntegralTest_");
    TString nameBack = fit -> GetName(); nameBack .ReplaceAll("fit_","histIntegralBack_");
    TString nameErrr = fit -> GetName(); nameErrr.ReplaceAll("fit_","histIntegralErrr_");
    TString title = Form("[RUN %04d] (%d) Count in contour;S = Contour amplitude [A];Count",fCurrentRunNumber,idx);
    TH1D *histData = (TH1D*) fHistDataArray -> At(idx);
    TH1D *histFitG = (TH1D*) fHistFitGArray -> At(idx);
    TH1D *histBack = (TH1D*) fHistBackArray -> At(idx);
    TH1D *histTest = (TH1D*) fHistTestArray -> At(idx);
    TH1D *histErrr = (TH1D*) fHistErrrArray -> At(idx);
    if (histData==nullptr)
    {
        gROOT -> cd();
        histData = new TH1D(nameData,title,fNumContours,0,1);
        histFitG = new TH1D(nameFitG,title,fNumContours,0,1);
        histBack = new TH1D(nameBack,title,fNumContours,0,1);
        histTest = new TH1D(nameTest,title,fNumContours,0,1);
        histErrr = new TH1D(nameErrr,title,fNumContours,0,1);
        fHistDataArray -> Add(histData);
        fHistFitGArray -> Add(histFitG);
        fHistBackArray -> Add(histBack);
        fHistTestArray -> Add(histTest);
        fHistErrrArray -> Add(histErrr);
        histData -> SetFillColor(19);
        histData -> SetLineWidth(2);
        histData -> SetLineColor(kBlack);
        histData -> GetXaxis() -> SetTitleOffset(1.2);
        histFitG -> SetLineColor(kRed);
        histFitG -> SetLineWidth(2);
        histFitG -> SetLineStyle(2);
        histBack -> SetLineColor(kBlue);
        histBack -> SetLineWidth(2);
        histBack -> SetLineStyle(1);
    }
    else {
        histData -> Reset("ICES");
        histFitG -> Reset("ICES");
        histBack -> Reset("ICES");
        histTest -> Reset("ICES");
        histErrr -> Reset("ICES");
        histData -> SetTitle(title);
        histFitG -> SetTitle(title);
        histBack -> SetTitle(title);
        histTest -> SetTitle(title);
        histErrr -> SetTitle(title);
        histData -> SetName(nameData);
        histFitG -> SetName(nameFitG);
        histBack -> SetName(nameBack);
        histTest -> SetName(nameTest);
        histErrr -> SetName(nameErrr);
    }
    auto draw = new LKDrawing();
    draw -> SetCanvasMargin(0.15,0.05,0.1,0.1);
    draw -> SetOptStat(0);
    draw -> SetAutoMax();
    draw -> SetCreateLegend();
    draw -> Add(histData,"hist","data");
    if (fitContanminent!=nullptr)
        draw -> Add(histBack,"same hist","contaminent");
    draw -> Add(histFitG,"same","fit");
    //draw -> Add(histTest,"same","test");
    //draw -> Add(histErrr,"same text",".");
    draw -> AddLegendLine(Form("A=%.2f",amplit));
    draw -> AddLegendLine(Form("x=%.2f",valueX));
    draw -> AddLegendLine(Form("#sigma_{x}=%.2f",sigmaX));
    draw -> AddLegendLine(Form("y=%.2f",valueY));
    draw -> AddLegendLine(Form("#sigma_{y}=%.2f",sigmaY));
    draw -> AddLegendLine(Form("#theta=%.1f deg.",thetaR*TMath::RadToDeg()));
    double wx = hist -> GetXaxis() -> GetBinWidth(1);
    double wy = hist -> GetYaxis() -> GetBinWidth(1);
    double binA = wx*wy;
    double dc = (1./fNumContours);
    //double countFullG = Integral2DGaussian(fit, 0) / binA;
    double countFullG = 1;
    for (double contourScale=0; contourScale<1; contourScale+=dc) {
        auto graphC = GetContourGraph(contourScale*amplit, amplit, valueX, sigmaX, valueY, sigmaY, thetaR);
        graphC -> SetName(Form("contourGraph_%.2f",contourScale));
        if (contourScale==fFinalContourAScale) fFinalContourGraph = graphC;
        double countFitG = Integral2DGaussian(fit, contourScale*amplit);
        countFitG = countFitG / binA;
        histFitG -> SetBinContent(histFitG->GetXaxis()->FindBin(contourScale+0.5*dc),countFitG/countFullG);
        double countTest = IntegralInsideGraph(hist, graphC, fit);
        countTest = countTest;
        histTest -> SetBinContent(histTest->GetXaxis()->FindBin(contourScale+0.5*dc),countTest/countFullG);
        if (fitContanminent!=nullptr) {
            double countBack = IntegralInsideGraph(hist, graphC, fitContanminent);
            countBack = countBack;
            histBack -> SetBinContent(histBack->GetXaxis()->FindBin(contourScale+0.5*dc),countBack/countFullG);
        }
        if (contourScale!=0) {
            double countHist = IntegralInsideGraph(hist, graphC);
            histData -> SetBinContent(histData->GetXaxis()->FindBin(contourScale+0.5*dc),countHist/countFullG);
            histErrr -> SetBinContent(histErrr->GetXaxis()->FindBin(contourScale+0.5*dc),abs(countHist-countFitG)/countFitG);
        }
    }
    return draw;
}

TF2* LKBeamPID::Fit2DGaussian(TH2D *hist, int idx, double valueX, double valueY, double sigmaX, double sigmaY, double theta)
{
    TF2 *fit = new TF2(Form("fit_%04d_%d", fCurrentRunNumber, idx), fFormulaRotated2DGaussian, valueX-fSigDist*sigmaX,valueX+fSigDist*sigmaX, valueY-fSigDist*sigmaY,valueY+fSigDist*sigmaY);
    double amplit = hist -> GetBinContent(hist->GetXaxis()->FindBin(valueX),hist->GetYaxis()->FindBin(valueY));
    fit -> SetParameter(0, amplit);
    fit -> SetParameter(1, valueX);
    fit -> SetParameter(3, valueY);
    fit -> SetParameter(2, sigmaX);
    fit -> SetParameter(4, sigmaY);
    fit -> SetParameter(5, theta*TMath::DegToRad());
    fit -> SetParLimits(0, amplit*0.5, amplit*2);
    fit -> SetParLimits(1, valueX-0.5*sigmaX, valueX+0.5*sigmaX);
    fit -> SetParLimits(3, valueY-0.5*sigmaY, valueY+0.5*sigmaY);
    fit -> SetParLimits(2, sigmaX*0.5, sigmaX*1.2);
    fit -> SetParLimits(4, sigmaY*0.5, sigmaY*1.2);
    fit -> SetParLimits(5, 0.*TMath::Pi(), 0.25*TMath::Pi());
    fit -> SetLineColor(kRed);
    hist -> Fit(fit,"QBR0");
    fit -> SetContour(3);
    e_cout << "   " << fit -> GetName() << ": " << std::left
        << setw(16) << Form("amp=%f,", fit->GetParameter(0))
        << setw(28) << Form("x=(%f, %f),", fit->GetParameter(1), fit->GetParameter(2))
        << setw(28) << Form("y=(%f, %f),", fit->GetParameter(3), fit->GetParameter(4))
        << setw(18) << Form("theta=%f,", fit->GetParameter(5)*TMath::RadToDeg())
        << endl;
    return fit;
}

TGraph *LKBeamPID::GetContourGraph(double contoA, double amplit, double valueX, double sigmaX, double valueY, double sigmaY, double thetaR)
{
    double Rx = sigmaX * sqrt(-2 * log(contoA / amplit));
    double Ry = sigmaY * sqrt(-2 * log(contoA / amplit));

    auto graph = new TGraph();
    graph -> SetMarkerStyle(20);

    for (double theta=0; theta<=2*TMath::Pi(); theta+=0.1) {
        double pointX = valueX + Rx * cos(theta);
        double pointY = valueY + Ry * sin(theta);
        TVector3 point(pointX-valueX,pointY-valueY,0);
        point.RotateZ(thetaR);
        graph -> SetPoint(graph->GetN(),point.X()+valueX,point.Y()+valueY);
    }
    graph -> SetPoint(graph->GetN(),graph->GetPointX(0),graph->GetPointY(0));
    return graph;
}

double LKBeamPID::IntegralInsideGraph(TH2D* hist, TGraph* graph, bool justCount)
{
    double integral = 0;
    int nx = hist -> GetXaxis() -> GetNbins();
    int ny = hist -> GetYaxis() -> GetNbins();
    double wx = hist -> GetXaxis() -> GetBinWidth(1);
    double wy = hist -> GetYaxis() -> GetBinWidth(1);
    double binA = wx*wy;
    if (justCount) binA = 1;
    for (auto xbin=1; xbin<=nx; ++xbin) {
        double xvalue = hist -> GetXaxis() -> GetBinCenter(xbin);
        for (auto ybin=1; ybin<=ny; ++ybin) {
            double yvalue = hist -> GetYaxis() -> GetBinCenter(ybin);
            if (graph -> IsInside(xvalue,yvalue)) {
                double value = hist -> GetBinContent(xbin,ybin);
                integral += value*binA;
            }
        }
    }
    return integral;
}

double LKBeamPID::IntegralInsideGraph(TH2D* hist, TGraph* graph, TF2 *f2, bool justCount)
{
    double integral = 0;
    int nx = hist -> GetXaxis() -> GetNbins();
    int ny = hist -> GetYaxis() -> GetNbins();
    double wx = hist -> GetXaxis() -> GetBinWidth(1);
    double wy = hist -> GetYaxis() -> GetBinWidth(1);
    double binA = wx*wy;
    if (justCount) binA = 1;
    for (auto xbin=1; xbin<=nx; ++xbin) {
        double xvalue = hist -> GetXaxis() -> GetBinCenter(xbin);
        for (auto ybin=1; ybin<=ny; ++ybin) {
            double yvalue = hist -> GetYaxis() -> GetBinCenter(ybin);
            if (graph -> IsInside(xvalue,yvalue)) {
                double value = f2 -> Eval(xvalue,yvalue);
                integral += value*binA;
            }
        }
    }
    return integral;
}

double LKBeamPID::Integral2DGaussian(double amplitude, double sigma_x, double sigma_y, double contoA)
{
    return amplitude*2*TMath::Pi()*sigma_x*sigma_y*(1-contoA/amplitude);
}

double LKBeamPID::Integral2DGaussian(TF2 *f2, double contoA)
{
    return Integral2DGaussian(f2->GetParameter(0), f2->GetParameter(2), f2->GetParameter(4), contoA);
}

void LKBeamPID::CollectRootFiles(std::vector<TString> &listGenFile, const char *dataPath, const TString &format)
{
    e_info << "Looking for data files(" << format << ") in " << dataPath << endl;
    void *dirp = gSystem->OpenDirectory(dataPath);
    if (!dirp) {
        e_warning << dataPath << " do not exist!" << endl;
        return;
    }

    const char *entry;
    while ((entry = gSystem->GetDirEntry(dirp))) {
        TString fileName = entry;
        if (fileName.Index(".")==0 || fileName == "..") continue;
        TString fullPath = TString(dataPath) + "/" + fileName;
        fullPath.ReplaceAll("//","/");
        if (gSystem->AccessPathName(fullPath, kFileExists)) continue;
        if (gSystem->OpenDirectory(fullPath)) {
            CollectRootFiles(listGenFile,fullPath, format);
        } else {
            if (fileName.EndsWith(format))
                listGenFile.push_back(fullPath);
        }
    }
    gSystem->FreeDirectory(dirp);
}

void LKBeamPID::SetRunNumber(int run)
{
    fCurrentRunNumber = run;
}

void LKBeamPID::SetGausFitRange(double sigDist)
{
    if (sigDist<0) {
        e_note << "Enter fit range from 0 to 5 (1 default): ";
        TString inputString;
        cin >> inputString;
        inputString = inputString.Strip(TString::kBoth);
        sigDist = inputString.Atof();
    }
    fSigDist = sigDist;
    e_cout << "   " << fSigDist << endl;
}

void LKBeamPID::SetSValue(double scale)
{
    if (scale<0) {
        e_note << "Enter fit range from 0.1 to 0.9 (1 default): ";
        TString inputString;
        cin >> inputString;
        inputString = inputString.Strip(TString::kBoth);
        scale = inputString.Atof();
    }
    fFinalContourAScale = scale;
    e_cout << "   " << fFinalContourAScale << endl;
}

void LKBeamPID::SaveConfiguration()
{
}

void LKBeamPID::PrintBinning() { fBnn2.Print(); fBnn3.Print(); }
void LKBeamPID::ResetBinning() { fBnn2.SetNMM(800,-1025,-875,800,0,200); fBnn3.SetNMM(800,-1025,-875,800,0,200); CreateAndFillHistogram(1); }
void LKBeamPID::SetBinningX(int nx, double x1, double x2, int fill) { fBnn2.SetXNMM(nx,x1,x2); fBnn3.SetXNMM(nx,x1,x2); if (fill) CreateAndFillHistogram(1); }
void LKBeamPID::SetBinningY(int ny, double y1, double y2, int fill) { fBnn2.SetYNMM(ny,y1,y2); fBnn3.SetYNMM(ny,y1,y2); if (fill) CreateAndFillHistogram(1); }
void LKBeamPID::SetRangeX(double x1, double x2, int fill) { fBnn2.SetXMM(x1,x2); fBnn3.SetXMM(x1,x2); if (fill) CreateAndFillHistogram(1); }
void LKBeamPID::SetRangeY(double y1, double y2, int fill) { fBnn2.SetYMM(y1,y2); fBnn3.SetYMM(y1,y2); if (fill) CreateAndFillHistogram(1); }
void LKBeamPID::SetBinning(int nx, double x1, double x2, int ny, double y1, double y2) { SetBinningX(nx,x1,x2,0); SetBinningY(ny,y1,y2,0); CreateAndFillHistogram(1); }
void LKBeamPID::SetBinning(double x1, double x2, double y1, double y2) { SetRangeX(x1,x2,0); SetRangeY(y1,y2,0); CreateAndFillHistogram(1); }
void LKBeamPID::SetXBinSize(double w, int fill) { fBnn2.SetWX(w); fBnn3.SetWX(w); if (fill) CreateAndFillHistogram(1); }
void LKBeamPID::SetYBinSize(double w, int fill) { fBnn2.SetWY(w); fBnn3.SetWY(w); if (fill) CreateAndFillHistogram(1); }
void LKBeamPID::SetBinNX(double n, int fill) { fBnn2.SetNX(n); fBnn3.SetNX(n); if (fill) CreateAndFillHistogram(1); }
void LKBeamPID::SetBinNY(double n, int fill) { fBnn2.SetNY(n); fBnn3.SetNY(n); if (fill) CreateAndFillHistogram(1); }
void LKBeamPID::SaveBinning() {
    if (!fHistPID) return;
    fBnn2.SetBinning(fHistPID,true);
    fBnn3.SetBinning(fHistPID,true);
    CreateAndFillHistogram(1);
}
