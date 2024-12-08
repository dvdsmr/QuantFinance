#include "options.h"
#include "compounding.h"
#include "Random.h"
#include "distributions.h"
#include "securities.h"
#include "sdes.h"
#include "saving.h"
#include "xyvals.h"
#include "out.h"
#include "adam.h"
#include "numpy.h"
#include "copola.h"
#include "volatility.h"
#include "fft.h"
#include "calibrate.h"
#include "pso.h"
#include "reading.h"
#include "risk.h"
#include <iostream>
#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

auto main() -> int
{

	//LabeledTable volSurface{ Volatility::Surface::testCalibration() };
	//Saving::write_labeledTable_to_csv("Data/ArtificalVolSurface.csv", volSurface);

	//FFT::UnitTests::separateModes();
	//FFT::UnitTests::dft();
	//FFT::UnitTests::pricingfft();

	//Calibrate::Heston::test();

	//Calibrate::BSM::test();
	//Calibrate::MertonJump::test();

	//Calibrate::Bachelier::test();

	//Calibrate::VarianceGamma::test();

	//testPSO();

	//SDE::Testing::saveMCsamples();
	//SDE::Testing::saveMertonJumpPaths();

	//SDE::Testing::saveHestonPaths();

	//SDE::Testing::saveVarianceGammaPaths();

	// save loss curve for BSM calibration
	

	//Options::Pricing::CEV::test();

	Options::Pricing::BSM::testMonteCarlo();

	Options::Pricing::Bachelier::testMonteCarlo();

	Options::Pricing::CEV::testMonteCarlo();

	/*
	double riskFreeReturn{ 0.003 };
	double dividendYield{ 0.0 };
	double maturity{ 0.0136986 };
	double strike{ 320. };
	double spot{ 320.72 };
	double truePrice{ 10.35 };

	Calibrate::BSM::saveLossShape(riskFreeReturn, dividendYield, maturity, strike, spot, truePrice);
	Calibrate::BSM::calibrateToRealData("TSLA", 338.74);
	
	*/
	
	
	

	//Risk::testSampleRiskMeasures();

	return 0;
}