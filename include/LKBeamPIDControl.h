#ifndef LKBEAMPIDCONTROL_HH
#define LKBEAMPIDCONTROL_HH

#include <TGClient.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TApplication.h>
#include <RQ_OBJECT.h>
#include <iostream>
#include "LKBeamPID.h"

class LKBeamPIDControl : public TGMainFrame {
    RQ_OBJECT("LKBeamPIDControl")

    public:
        LKBeamPID *fBeamPID = nullptr;

    public:
        enum class InputMode {
            None,
            SelectFile,
            ChangeSigmaDist
        };

        LKBeamPIDControl(LKBeamPID *beamPID, UInt_t w=780, UInt_t h=750);
        LKBeamPIDControl(UInt_t w=780, UInt_t h=750);
        void OnSelectFile();
        void OnChangeSigmaDist();
        void OnEnter();

    private:
        void RequireInput(InputMode mode) {
            fInputMode = mode;
            fBtnEnter->SetEnabled(kTRUE);
            fNumEntry->GetNumberEntry()->SetFocus();
        }

        void ClearInputMode() {
            fInputMode = InputMode::None;
            fBtnEnter->SetEnabled(kFALSE);
        }

        void Info(const char* msg) { std::cout << "[Panel] " << msg << std::endl; }

        // UI elements
        TGNumberEntry *fNumEntry{nullptr};
        TGTextButton  *fBtnEnter{nullptr};

        TGTextButton *fBtnHelp{nullptr};
        TGTextButton *fBtnPrintBinning{nullptr};
        TGTextButton *fBtnResetBinning{nullptr};
        TGTextButton *fBtnSaveBinning{nullptr};
        TGTextButton *fBtnSetBinningSize{nullptr};
        TGTextButton *fBtnSetSValue{nullptr};
        TGTextButton *fBtnSetFitRangeSigma{nullptr};
        TGTextButton *fBtnSaveConfig{nullptr};

        TGTextButton *fBtnListFiles{nullptr};
        TGTextButton *fBtnSelectFile{nullptr};
        TGTextButton *fBtnClickSelectPoints{nullptr};
        TGTextButton *fBtnRedrawPlot{nullptr};
        TGTextButton *fBtnRedrawSelectPoints{nullptr};
        TGTextButton *fBtnFitTotal{nullptr};
        TGTextButton *fBtnMakeSummary{nullptr};

        InputMode fInputMode;

    ClassDef(LKBeamPIDControl, 1)
};

#endif
