//#include "include/LKCompiled.h"

void draw_beam_pid()
{
    LKBeamPID pid;
    pid.SetBinning(120,-946,-907, 120, 109, 147);
    pid.ListFiles();
    pid.SelectFile(0);
    pid.DrawPoints(vector<vector<double>>{vector<double>{-927.581, 130.787},vector<double>{-934.872649, 125.220195},vector<double>{-927.581, 124}});
    pid.FitTotal();
    pid.MakeSummary();
}
