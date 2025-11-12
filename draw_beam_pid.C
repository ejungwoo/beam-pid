//#include "include/LKCompiled.h"

void draw_beam_pid()
{
    if (0) {
        auto file = new TFile("summary/run_0083.root");
        file -> Get("pid") -> Draw();
    }
    else if (0) {
        auto file = new TFile("summary/run_0083.root");
        file -> Get("pid") -> Draw();
        LKBeamPID pid;
        pid.SetRunNumber(83);
        pid.UseCurrentgPad();
        pid.SelectCenters(vector<vector<double>>{vector<double>{-927.581, 130.787},vector<double>{-934.872649, 125.220195},vector<double>{-927.581, 124}});
        pid.FitTotal();
        pid.MakeSummary();
    }
    else {
        auto pid = new LKBeamPIDControl;
        return;
        pid -> SetBinning(120,-946,-907, 120, 109, 147);
        pid -> ListFiles();
        pid -> SelectFile(0);
        pid -> SelectCenters(vector<vector<double>>{vector<double>{-927.581, 130.787},vector<double>{-934.872649, 125.220195},vector<double>{-927.581, 124}});
        pid -> FitTotal();
        pid -> MakeSummary();
    }
}
