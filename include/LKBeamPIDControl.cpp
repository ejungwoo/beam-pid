#include "LKBeamPIDControl.h"

ClassImp(LKBeamPIDControl)
        
LKBeamPIDControl::LKBeamPIDControl(UInt_t w, UInt_t h)
    : LKBeamPIDControl(new LKBeamPID, w, h)
{
}

LKBeamPIDControl::LKBeamPIDControl(LKBeamPID *beamPID, UInt_t w, UInt_t h)
    : TGMainFrame(gClient->GetRoot(), w, h), fInputMode(InputMode::None)
{
    fBeamPID = beamPID;

    SetCleanup(kDeepCleanup);

    auto *vMain = new TGVerticalFrame(this);
    AddFrame(vMain, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 8,8,8,8));

    // ---------------------------
    // 상단: 숫자 입력 + Enter 버튼
    // ---------------------------
    auto *topFrame = new TGHorizontalFrame(vMain);
    vMain->AddFrame(topFrame, new TGLayoutHints(kLHintsExpandX, 0,0,10,10));

    topFrame->AddFrame(new TGLabel(topFrame, "Input:"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0,6,0,0));

    fNumEntry = new TGNumberEntry(topFrame, 0.0, 8, -1,
            TGNumberFormat::kNESReal,
            TGNumberFormat::kNEAAnyNumber);
    fNumEntry->Resize(100, fNumEntry->GetDefaultHeight());
    topFrame->AddFrame(fNumEntry, new TGLayoutHints(kLHintsLeft | kLHintsCenterY));

    fBtnEnter = new TGTextButton(topFrame, "Enter");
    fBtnEnter->SetEnabled(kFALSE);
    topFrame->AddFrame(fBtnEnter, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 6,0,0,0));
    fBtnEnter->Connect("Clicked()", "LKBeamPIDControl", this, "OnEnter()");
    fNumEntry->GetNumberEntry()->Connect("ReturnPressed()", "LKBeamPIDControl", this, "OnEnter()");

    // ---------------------------
    // 버튼 영역: 2열 구성
    // ---------------------------
    auto *hMain = new TGHorizontalFrame(vMain);
    vMain->AddFrame(hMain, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    auto *col1 = new TGVerticalFrame(hMain);
    auto *col2 = new TGVerticalFrame(hMain);
    hMain->AddFrame(col1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4,4,0,0));
    hMain->AddFrame(col2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4,4,0,0));

    auto *btnLH = new TGLayoutHints(kLHintsExpandX, 0,0,3,3);

    auto mkBtn = [&](TGVerticalFrame* parent, const char* label) {
        auto* b = new TGTextButton(parent, label);
        b->SetTextJustify(kTextLeft); // ← 이 한 줄 추가!
        const TGFont *font = gClient->GetFont("-adobe-helvetica-bold-r-*-*-18-*-*-*-*-*-*-*");
        if (font) b->SetFont(font->GetFontStruct());
        parent->AddFrame(b, btnLH);
        return b;
    };

    // 왼쪽 열
    fBtnHelp             = mkBtn(col2, "c1  Help");
    fBtnPrintBinning     = mkBtn(col2, "c2  Print binning");
    fBtnResetBinning     = mkBtn(col2, "c3  Reset binning");
    fBtnSaveBinning      = mkBtn(col2, "c4  Save binning");
    fBtnSetBinningSize   = mkBtn(col2, "c5  Set x-bin width");
    fBtnSetBinningSize   = mkBtn(col2, "c6  Set y-bin width");
    fBtnSetSValue        = mkBtn(col2, "c7  Set S value");
    fBtnSetFitRangeSigma = mkBtn(col2, "c8  Set fit range (sigma)");
    fBtnSaveConfig       = mkBtn(col2, "c9  Save configuration");

    // 오른쪽 열
    fBtnListFiles           = mkBtn(col1, "a1  List files");
    fBtnSelectFile          = mkBtn(col1, "a2  Select file");
    fBtnClickSelectPoints   = mkBtn(col1, "a3  Click select points");
    fBtnFitTotal            = mkBtn(col1, "a4  Fit total");
    fBtnMakeSummary         = mkBtn(col1, "a5  Make summary");
    fBtnRedrawPlot          = mkBtn(col1, "b1  Redraw plot");
    fBtnRedrawSelectPoints  = mkBtn(col1, "b2  Redraw & select points");

    // 연결
    fBtnSelectFile->Connect("Clicked()", "LKBeamPIDControl", this, "OnSelectFile()");
    fBtnSetFitRangeSigma->Connect("Clicked()", "LKBeamPIDControl", this, "OnChangeSigmaDist()");
    fBtnEnter->Connect("Clicked()", "LKBeamPIDControl", this, "OnEnter()");

    SetWindowName("Beam PID Control");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

void LKBeamPIDControl::OnSelectFile() {
    Info("Select file → 숫자 입력 대기");
    RequireInput(InputMode::SelectFile);
}

void LKBeamPIDControl::OnChangeSigmaDist() {
    Info("Change sigma dist → 숫자 입력 대기");
    RequireInput(InputMode::ChangeSigmaDist);
}

void LKBeamPIDControl::OnEnter() {
    if (!fBtnEnter->IsEnabled()) return;
    double val = fNumEntry->GetNumber();
    switch (fInputMode) {
        case InputMode::SelectFile:
            Info(Form("Select file index: %.3f", val));
            break;
        case InputMode::ChangeSigmaDist:
            Info(Form("Sigma dist changed to %.3f", val));
            break;
        default:
            Info(Form("Enter pressed: %.3f", val));
            break;
    }
    ClearInputMode();
}

