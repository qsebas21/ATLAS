#include <iostream>
#include <string>
#include "TMath.h"
#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TLegend.h"
#include "Math/ProbFunc.h"


using namespace std;

float chisquareNBins(TH1F * data, TH1F * mc);

void SetStyle();

int main(){

	// At this point, you want to first test the agreement between data and monte carlo.

	//////////////////////////////////////////////////////////////////
	// To do: 1. Implement Chi^2 Tets in the function chisquareNBins
    //        2. Quantify data-mc agreement with the implemented function
	//		  3. Quantify data-mc with added uncertainities. You have to modify the function for that
	////////////////////////////////////////////////////////////////////////////

    std::string basePath = "";
    std::cout << "Vorher" << std::endl;
    TFile* file_MC = new TFile((basePath+"analysis.root").c_str(),"READ");
    std::cout << "Nachher" << std::endl;
    
    TH1F * h_background = (TH1F*) file_MC->Get("background");
    std::cout  << h_background->GetNbinsX() << std::endl;
    
    TH1F * h_data = (TH1F*) file_MC->Get("data");
    std::cout << h_data->GetNbinsX() << std::endl;
    
    // ** calculate chi square now with the given function
    float chi_square = chisquareNBins(h_data, h_background);
    std::cout <<"Data/MC agreement (Chi^2) is: " << chi_square << std::endl;
    double p_value = ROOT::Math::chisquared_cdf_c(chi_square,h_data->GetNbinsX() );
    std::cout << "p-Value is: " << p_value << std::endl;
}    

float chisquareNBins(TH1F * data, TH1F * mc){
    ////////////////////////////////////////////
    // To do: Add code here to have Chi^2 Test for data vs MC
    // Make sure to run the Chi^2 Test once without any systematic uncertainties and once with 14% systematic unceratintes 
    //
    bool includeSysUncertainty = true;
    float chi2 = 0.0;
    int nbins = data->GetNbinsX();

    if (nbins!= mc->GetNbinsX()) {
	std::cerr << "Error: Histograms have different bins! :(" << std::endl;
        return -1;
    }

    std::cout << "Computing chi-squared " << (includeSysUncertainty ? "with" : "without") << " systematic uncertainties" << std::endl;

    for (int i=1; i<=nbins; ++i){
	float d = data->GetBinContent(i);
	float m = mc->GetBinContent(i);
	float err_d = data->GetBinError(i);
	float err_m = mc->GetBinContent(i);

	if (includeSysUncertainty) {
            float sys = 0.14*d;
	    err_d = sqrt(err_d * err_d + sys * sys);
	}

	float total_error_sq = err_d * err_d + err_m * err_m;

	if (total_error_sq <= 0) {
	    std::cout << "Skipping bin " << i << " (zero uncertainty)" << std::endl;
	    continue;
	}

	float contribution = (d-m)* (d-m) / total_error_sq;
	chi2 += contribution;

	std::cout << "Bin " << i << ": data =" << d << ", mc = " << m << ", chi2_contrib = " << contribution << std::endl;
    }
    return chi2;
}

void SetStyle() {
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetTitleXOffset(1.3);
  gStyle->SetTitleYOffset(1.3);
  gStyle->SetTextSize(0.05);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetLabelSize(0.05,"z");
  gStyle->SetTitleSize(0.05,"z");
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth(2);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
}
