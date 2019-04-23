#ifndef HEADER_H
#define HEADER_H

#define NUM_PMT 494
#define NUM_Z 7
#define NUM_Y 4
#define GEOMETRY_FILE "LZ_geometry.dump_icd"
#define EE_HISTO_FILE "EE_out.root"
#define NR_HISTO_FILE "NR_out.root"

using namespace std;

static Int_t** PMTMap;
static Double_t** PMTCoords;
static Double_t *weights;

static TH1F *histogram[494][7][4][2];

//Arrays holding values of y and z points
Double_t zPoints[NUM_Z] = {1, 242, 484, 726, 968, 1210, 1440};
Double_t yPoints[NUM_Y] = {0, 220, 440, 660};

//Arrays holding parts of file names
string yVals[] = {"r00_", "r22_", "r44_", "r66_"};
string zVals[] = {"z0001", "z0242", "z0484", "z0726", "z0968", "z1210", "z1440"};
string type[] = {"EE_", "NR_"};

//Struct to hold all the info associated with a pair of y,z coords
typedef struct Point
{
  Int_t y;
  Int_t z;
  Int_t yIndex;
  Int_t zIndex;
} Point;

static Point **points;

//TODO - remove this stuff
TStopwatch *evtClock = new TStopwatch();
Double_t timeElapsed = 0;
static Int_t mappingCount = 0;

#endif
