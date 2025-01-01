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
#include "optionClass.h"
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

	BSMParams bsmParams{ 0.2 };
	Securities::ModelStock bsmStock(100., bsmParams);

	MertonJumpParams mjParams{ };
	Securities::ModelStock mjStock(100., mjParams);
	MertonJumpParams returnParams{ mjStock.getParams() };

	std::cout << returnParams.vol << " " << returnParams.meanJumpSize << " " << returnParams.stdJumpSize;

	XYVals xyvals{ mjStock.path(1.,100,0.05) };
	std::cout << "\nPrice after simulation: " << xyvals.m_yVals.back();

	VarianceGammaParams vgparams{};
	Securities::ModelStock vgStock(100., vgparams);
	XYVals vgXyvals{ vgStock.path(1.,100,0.05) };
	std::cout << "\nVG model\nPrice after simulation: " << vgXyvals.m_yVals.back();

	XYVals mcSamples{ vgStock.monteCarlo(1.,100,0.05) };
	std::cout << "\nOne MC sample: " << mcSamples.m_yVals.back();
	
	/*
	Securities::Stock underlying(100.);
	Option option(Option::PayoffType::call,
		Option::ExerciseType::european,
		Option::Position::longPosition,
		underlying,
		1.);

	Option option2(Option::PayoffType::call,
		Option::ExerciseType::european,
		Option::Position::longPosition,
		Securities::Stock(100.),
		1.);
	*/

	std::shared_ptr<Securities::AbstractStock> mjStockPtr = std::make_shared<Securities::ModelStock<MertonJumpParams>>(100., mjParams);
	Option option3(Option::PayoffType::call,
		Option::ExerciseType::european,
		Option::Position::longPosition,
		mjStockPtr,
		1.);


	//std::cout << option.get_underlying().getSpot();
	//std::cout << option2.get_underlying().getSpot();
	std::cout << option3.get_underlying()->getSpot();

	XYVals xyvals555{ option3.get_underlying()->path(1.,100,0.05) };
	std::cout << "\nPrice after simulation: " << xyvals555.m_yVals.back();



	//Calibrate::Bachelier::test();

	//Calibrate::VarianceGamma::test();

	//testPSO();

	//SDE::Testing::saveMCsamples();
	//SDE::Testing::saveMertonJumpPaths();

	//SDE::Testing::saveHestonPaths();

	//SDE::Testing::saveVarianceGammaPaths();

	// save loss curve for BSM calibration
	

	//Options::Pricing::CEV::test();

	//Options::Pricing::BSM::testMonteCarlo();
	//Options::Pricing::Bachelier::testMonteCarlo();
	//Options::Pricing::CEV::testMonteCarlo();
	//Options::Pricing::Heston::testPricing();
	//Options::Pricing::MertonJump::testPricing();
	//Options::Pricing::VarianceGamma::testPricing();

	//SDE::Testing::saveVarianceGammaPaths();

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