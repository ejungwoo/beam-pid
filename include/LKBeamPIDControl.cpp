#include "LKBeamPIDControl.h"

ClassImp(LKBeamPIDControl)
        
LKBeamPIDControl::LKBeamPIDControl(UInt_t w, UInt_t h)
    : TGMainFrame(gClient->GetRoot(), w, h), fInputMode(InputMode::None), LKBeamPID()
{
    SetCleanup(kDeepCleanup);

    auto *vMain = new TGVerticalFrame(this);
    AddFrame(vMain, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 8,8,8,8));

    // Number + Enter
    auto *topFrame = new TGHorizontalFrame(vMain);
    vMain->AddFrame(topFrame, new TGLayoutHints(kLHintsExpandX, 0,0,10,10));

    topFrame->AddFrame(new TGLabel(topFrame, "Input:"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0,6,0,0));

    fNumEntry = new TGNumberEntry(topFrame, 0.0, 8, -1,
            TGNumberFormat::kNESReal,
            TGNumberFormat::kNEAAnyNumber);
    fNumEntry->Resize(100, fNumEntry->GetDefaultHeight());
    topFrame->AddFrame(fNumEntry, new TGLayoutHints(kLHintsLeft | kLHintsCenterY));

    fButtonEnter = new TGTextButton(topFrame, "Enter");
    fButtonEnter->SetEnabled(kFALSE);
    topFrame->AddFrame(fButtonEnter, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 6,0,0,0));
    fButtonEnter->Connect("Clicked()", "LKBeamPIDControl", this, "PressedEnter()");
    fNumEntry->GetNumberEntry()->Connect("ReturnPressed()", "LKBeamPIDControl", this, "PressedEnter()");

    // Buttons
    auto *hMain = new TGHorizontalFrame(vMain);
    vMain->AddFrame(hMain, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    auto *col1 = new TGVerticalFrame(hMain);
    auto *col2 = new TGVerticalFrame(hMain);
    hMain->AddFrame(col1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4,4,0,0));
    hMain->AddFrame(col2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4,4,0,0));

    auto *btnLH = new TGLayoutHints(kLHintsExpandX, 0,0,3,3);

    fHLColor = TColor::RGB2Pixel(255, 255, 204); // RGB for fHLColor
    fNmColor = gClient->GetResourcePool()->GetFrameBgndColor();

    auto mkBtn = [&](TGVerticalFrame* parent, const char* label, TString connectFunctionName="") {
        auto* b = new TGTextButton(parent, label);
        b->SetTextJustify(kTextLeft); // ← 이 한 줄 추가!
        const TGFont *font = gClient->GetFont("-adobe-helvetica-bold-r-*-*-18-*-*-*-*-*-*-*");
        if (font) b->SetFont(font->GetFontStruct());
        parent->AddFrame(b, btnLH);
        if (!connectFunctionName.IsNull()) b->Connect("Clicked()", "LKBeamPIDControl", this, connectFunctionName);
        return b;
    };

    // left
    fButtonListFiles       = mkBtn(col1, "&List files",               "PressedListFiles()");
    fButtonSelectFile      = mkBtn(col1, "&Select file",              "PressedSetFileNumber()");
    fButtonUseCurrentgPad  = mkBtn(col1, "&Use drawing on gPad",      "PressedUseCurrentgPad()");
    fButtonSelectCenters   = mkBtn(col1, "&Click select centers",     "PressedSelectCenters()");
    fButtonRedrawPlot      = mkBtn(col1, "Redraw &plot",              "PressedRedraw()");
    fButtonReselectCenters = mkBtn(col1, "&Redraw and select centers","PressedReselectCenters()");
    fButtonFitTotal        = mkBtn(col1, "&Fit total",                "PressedFitTotal()");
    fButtonMakeSummary     = mkBtn(col1, "&Make summary",             "PressedMakeSummary()");
    fButtonHelp            = mkBtn(col2, "Help",                      "PressedHelp()");
    fButtonPrintBinning    = mkBtn(col2, "Print binning",             "PressedPrintBinning()");
    fButtonResetBinning    = mkBtn(col2, "Reset binning",             "PressedResetBinning()");
    fButtonSaveBinning     = mkBtn(col2, "Save binning",              "PressedSaveBinning()");
    fButtonSetBinWidthX    = mkBtn(col2, "Set x-bin width",           "PressedSetXBinSize()");
    fButtonSetBinWidthY    = mkBtn(col2, "Set y-bin width",           "PressedSetYBinSize()");
    fButtonSetSValue       = mkBtn(col2, "Set S value",               "PressedSetSValue()");
    fButtonSetFitRange     = mkBtn(col2, "Set fit range (sigma)",     "PressedSetFitRange()");
    fButtonSetRunNumber    = mkBtn(col2, "Set run number",            "PressedSetRunNumber()");
    fButtonSaveConfig      = mkBtn(col2, "Save configuration",        "PressedSaveConfiguration()");

    // Bottom
    //auto *botFrame = new TGHorizontalFrame(vMain);
    //vMain->AddFrame(botFrame, new TGLayoutHints(kLHintsExpandX, 0,0,10,10));
    //vMain->AddFrame(botFrame, new TGLayoutHints(kLHintsExpandX));
    //auto buttonQuit = new TGTextButton(botFrame, "Quit");
    //botFrame->AddFrame(fButtonEnter, new TGLayoutHints(kLHintsExpandX, 0,6,3,3));
    //buttonQuit->Connect("Clicked()", "LKBeamPIDControl", this, "PressedQuit()");

    SetWindowName("Beam PID Control");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

void LKBeamPIDControl::ResetBB() 
{
    fButtonListFiles       -> ChangeBackground(fNmColor);
    fButtonSelectFile      -> ChangeBackground(fNmColor);
    fButtonUseCurrentgPad  -> ChangeBackground(fNmColor);
    fButtonSelectCenters   -> ChangeBackground(fNmColor);
    fButtonRedrawPlot      -> ChangeBackground(fNmColor);
    fButtonReselectCenters -> ChangeBackground(fNmColor);
    fButtonFitTotal        -> ChangeBackground(fNmColor);
    fButtonMakeSummary     -> ChangeBackground(fNmColor);
    fButtonHelp            -> ChangeBackground(fNmColor);
    fButtonPrintBinning    -> ChangeBackground(fNmColor);
    fButtonResetBinning    -> ChangeBackground(fNmColor);
    fButtonSaveBinning     -> ChangeBackground(fNmColor);
    fButtonSetBinWidthX    -> ChangeBackground(fNmColor);
    fButtonSetBinWidthY    -> ChangeBackground(fNmColor);
    fButtonSetSValue       -> ChangeBackground(fNmColor);
    fButtonSetFitRange     -> ChangeBackground(fNmColor);
    fButtonSetRunNumber    -> ChangeBackground(fNmColor);
    fButtonSaveConfig      -> ChangeBackground(fNmColor);
}

void LKBeamPIDControl::PressedListFiles()         { ResetBB(); fButtonListFiles       -> ChangeBackground(fHLColor); ListFiles();       }
void LKBeamPIDControl::PressedSetFileNumber()     { ResetBB(); fButtonSelectFile      -> ChangeBackground(fHLColor); RequireInput(InputMode::SetFileNumber); }
void LKBeamPIDControl::PressedUseCurrentgPad()    { ResetBB(); fButtonUseCurrentgPad  -> ChangeBackground(fHLColor); UseCurrentgPad();  }
void LKBeamPIDControl::PressedSelectCenters()     { ResetBB(); fButtonSelectCenters   -> ChangeBackground(fHLColor); SelectCenters();   }
void LKBeamPIDControl::PressedRedraw()            { ResetBB(); fButtonRedrawPlot      -> ChangeBackground(fHLColor); Redraw();          }
void LKBeamPIDControl::PressedReselectCenters()   { ResetBB(); fButtonReselectCenters -> ChangeBackground(fHLColor); ReselectCenters(); }
void LKBeamPIDControl::PressedFitTotal()          { ResetBB(); fButtonFitTotal        -> ChangeBackground(fHLColor); FitTotal();        }
void LKBeamPIDControl::PressedMakeSummary()       { ResetBB(); fButtonMakeSummary     -> ChangeBackground(fHLColor); MakeSummary();     }
void LKBeamPIDControl::PressedHelp()              { ResetBB(); fButtonHelp            -> ChangeBackground(fHLColor); Help();            }
void LKBeamPIDControl::PressedPrintBinning()      { ResetBB(); fButtonPrintBinning    -> ChangeBackground(fHLColor); PrintBinning();    }
void LKBeamPIDControl::PressedResetBinning()      { ResetBB(); fButtonResetBinning    -> ChangeBackground(fHLColor); ResetBinning();    }
void LKBeamPIDControl::PressedSaveBinning()       { ResetBB(); fButtonSaveBinning     -> ChangeBackground(fHLColor); SaveBinning();     }
void LKBeamPIDControl::PressedSetXBinSize()       { ResetBB(); fButtonSetBinWidthX    -> ChangeBackground(fHLColor); RequireInput(InputMode::SetXBinSize); }
void LKBeamPIDControl::PressedSetYBinSize()       { ResetBB(); fButtonSetBinWidthY    -> ChangeBackground(fHLColor); RequireInput(InputMode::SetYBinSize); }
void LKBeamPIDControl::PressedSetSValue()         { ResetBB(); fButtonSetSValue       -> ChangeBackground(fHLColor); RequireInput(InputMode::SetSValue); }
void LKBeamPIDControl::PressedSetFitRange()       { ResetBB(); fButtonSetFitRange     -> ChangeBackground(fHLColor); RequireInput(InputMode::SetFitRange); }
void LKBeamPIDControl::PressedSetRunNumber()      { ResetBB(); fButtonSetRunNumber    -> ChangeBackground(fHLColor); RequireInput(InputMode::SetRunNumber); }
void LKBeamPIDControl::PressedSaveConfiguration() { ResetBB(); fButtonSaveConfig      -> ChangeBackground(fHLColor); SaveConfiguration(); };

void LKBeamPIDControl::RequireInput(InputMode mode)
{
    fInputMode = mode;
    fButtonEnter->SetEnabled(kTRUE);
    fNumEntry->GetNumberEntry()->SetFocus();
    switch (fInputMode) {
        case InputMode::SetFileNumber:
            lk_info << "Enter file number" << endl;
            break;
        case InputMode::SetSValue:
            lk_info << "Enter X bin width" << endl;
            break;
        case InputMode::SetXBinSize:
            lk_info << "Enter X bin width" << endl;
            break;
        case InputMode::SetYBinSize:
            lk_info << "Enter S-value" << endl;
            break;
        case InputMode::SetFitRange:
            lk_info << "Enter fit range in sigma" << endl;
            break;
        case InputMode::SetRunNumber:
            lk_info << "Enter run number " << endl;
            break;
        default:
            break;
    }
}

void LKBeamPIDControl::PressedEnter()
{
    if (!fButtonEnter->IsEnabled()) return;
    double val = fNumEntry->GetNumber();
    switch (fInputMode) {
        case InputMode::SetFileNumber:
            lk_info << "Select file index : " << val << endl;
            SelectFile(int(val));
            break;
        case InputMode::SetSValue:
            lk_info << "S value changed to " << val << endl;
            SetSValue(val);
            break;
        case InputMode::SetXBinSize:
            lk_info << "x-bin size changed to " << val << endl;
            SetXBinSize(val);
            break;
        case InputMode::SetYBinSize:
            lk_info << "y-bin size changed to " << val << endl;
            SetYBinSize(val);
            break;
        case InputMode::SetFitRange:
            lk_info << "Fit range changed to " << val << " (*sigma)" << endl;
            SetGausFitRange(val);
            break;
        case InputMode::SetRunNumber:
            lk_info << "Fit range changed to " << val << " (*sigma)" << endl;
            SetRunNumber(int(val));
            break;
        default:
            break;
    }
    ClearInputMode();
}

void LKBeamPIDControl::PressedQuit()
{
    gApplication -> Terminate();
}

void LKBeamPIDControl::ClearInputMode() {
    fInputMode = InputMode::None;
    fButtonEnter->SetEnabled(kFALSE);
}
