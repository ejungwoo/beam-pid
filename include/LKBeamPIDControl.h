#ifndef LKBEAMPIDCONTROL_HH
#define LKBEAMPIDCONTROL_HH

#include <TGClient.h>
#include <TGResourcePool.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TApplication.h>
#include <RQ_OBJECT.h>
#include <iostream>
#include "TApplication.h"
#include "LKBeamPID.h"

class LKBeamPIDControl : public TGMainFrame, public LKBeamPID
{
    RQ_OBJECT("LKBeamPIDControl")

    public:
        enum class InputMode {
            None,
            SetFileNumber,
            SetXBinSize,
            SetYBinSize,
            SetSValue,
            SetFitRange,
            SetRunNumber,
        };

        LKBeamPIDControl(UInt_t w=900, UInt_t h=700);

        void ResetBB();

        void PressedListFiles();
        void PressedSetFileNumber();
        void PressedUseCurrentgPad();
        void PressedSelectCenters();
        void PressedRedraw();
        void PressedReselectCenters();
        void PressedFitTotal();
        void PressedMakeSummary();

        void PressedHelp();
        void PressedPrintBinning();
        void PressedResetBinning();
        void PressedSaveBinning();
        void PressedSetXBinSize();
        void PressedSetYBinSize();
        void PressedSetSValue();
        void PressedSetFitRange();
        void PressedSetRunNumber();
        void PressedSaveConfiguration();

        void PressedEnter();
        void PressedQuit();

    private:
        void RequireInput(InputMode mode);
        void ClearInputMode();

        // UI elements
        TGNumberEntry *fNumEntry = nullptr;
        TGTextButton  *fButtonEnter = nullptr;

        TGTextButton *fButtonListFiles       = nullptr;
        TGTextButton *fButtonSelectFile      = nullptr;
        TGTextButton *fButtonUseCurrentgPad  = nullptr;
        TGTextButton *fButtonSelectCenters   = nullptr;
        TGTextButton *fButtonRedrawPlot      = nullptr;
        TGTextButton *fButtonReselectCenters = nullptr;
        TGTextButton *fButtonFitTotal        = nullptr;
        TGTextButton *fButtonMakeSummary     = nullptr;
        TGTextButton *fButtonHelp            = nullptr;
        TGTextButton *fButtonPrintBinning    = nullptr;
        TGTextButton *fButtonResetBinning    = nullptr;
        TGTextButton *fButtonSaveBinning     = nullptr;
        TGTextButton *fButtonSetBinWidthX    = nullptr;
        TGTextButton *fButtonSetBinWidthY    = nullptr;
        TGTextButton *fButtonSetSValue       = nullptr;
        TGTextButton *fButtonSetFitRange     = nullptr;
        TGTextButton *fButtonSetRunNumber    = nullptr;
        TGTextButton *fButtonSaveConfig      = nullptr;

        InputMode fInputMode;

        int fHLColor = 0;
        int fNmColor = 0;

    ClassDef(LKBeamPIDControl, 1)
};

#endif
