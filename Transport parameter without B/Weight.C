#include <TApplication.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TSystem.h>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "Garfield/ViewMedium.hh"
#include "Garfield/AvalancheGrid.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/ComponentParallelPlate.hh"
#include "Garfield/FundamentalConstants.hh"
#include "Garfield/GeometrySimple.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Plotting.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/SolidBox.hh"
#include "Garfield/TrackHeed.hh"
#include "Garfield/ViewSignal.hh"

using namespace Garfield;

int main(int argc, char* argv[]) {
  TApplication app("app", &argc, argv);
  plottingEngine.SetDefaultStyle();

  // Setup the gas
  MediumMagboltz gas;
  gas.SetComposition("Ar", 100.);
  gas.SetPressure(750.);
  gas.SetTemperature(293.15);
  gas.GenerateGasTable(10);

  // Set field grid parameters explicitly
  gas.SetFieldGrid(100., 100000., 100, false);

  // Define field grid parameters
  std::vector<double> efields;
  std::vector<double> bfields = {0};
  std::vector<double> angles = {90.};

  gas.GetFieldGrid(efields, bfields, angles);
  const auto nE = efields.size();

  // Vectors for transport parameters
  std::vector<double> driftVelocities;
  std::vector<double> townsendCoeffs;
  std::vector<double> attachmentCoeffs;
  std::vector<double> longitudinalDiffusions;
  std::vector<double> transverseDiffusions;

  // Retrieve data and store in vectors
  for (size_t i = 0; i < nE; ++i) {
    double ve = 0., dl = 0., dt = 0., alpha = 0., eta = 0.;

    gas.GetElectronVelocityE(i, 0, 0, ve);
    gas.GetElectronLongitudinalDiffusion(i, 0, 0, dl);
    gas.GetElectronTransverseDiffusion(i, 0, 0, dt);
    gas.GetElectronTownsend(i, 0, 0, alpha);
    gas.GetElectronAttachment(i, 0, 0, eta);

    // Convert drift velocity from cm/ns to cm/us
    ve *= 1.e3;

    // Apply exponential to alpha and eta
    alpha = exp(alpha);
    eta = exp(eta);

    driftVelocities.push_back(ve);
    townsendCoeffs.push_back(alpha);
    attachmentCoeffs.push_back(eta);
    longitudinalDiffusions.push_back(dl);
    transverseDiffusions.push_back(dt);

    // Print parameters to console
    std::cout << "E_Field: " << efields[i] << " V/cm, Drift_Velocity: " << ve << " cm/μs, Townsend_Coeff: " << alpha << " 1/cm, Attachment_Coeff: " << eta << " √cm, Longitudinal_Diffusion: " << dl << " √cm, Transverse_Diffusion: " << dt << " √cm\n";
  }

  // Plot transport parameters
  TCanvas* c1 = new TCanvas("c1", "Transport Parameters", 800, 600);
  TGraph* grDriftVel = new TGraph(nE, &efields[0], &driftVelocities[0]);
  grDriftVel->SetTitle("Drift Velocity;Electric Field [V/cm];Velocity [cm/us]");
  grDriftVel->SetLineColor(kRed);
  grDriftVel->SetLineWidth(2);
  grDriftVel->Draw("AL");
  c1->SaveAs("DriftVelocity.png");

  TCanvas* c2 = new TCanvas("c2", "Townsend Coefficient", 800, 600);
  TGraph* grTownsend = new TGraph(nE, &efields[0], &townsendCoeffs[0]);
  grTownsend->SetTitle("Townsend Coefficient;Electric Field [V/cm];Coefficient [1/cm]");
  grTownsend->SetLineColor(kBlue);
  grTownsend->SetLineWidth(2);
  grTownsend->Draw("AL");
  c2->SaveAs("TownsendCoefficient.png");

  TCanvas* c3 = new TCanvas("c3", "Attachment Coefficient", 800, 600);
  TGraph* grAttachment = new TGraph(nE, &efields[0], &attachmentCoeffs[0]);
  grAttachment->SetTitle("Attachment Coefficient;Electric Field [V/cm];Coefficient [1/cm]");
  grAttachment->SetLineColor(kGreen);
  grAttachment->SetLineWidth(2);
  grAttachment->Draw("AL");
  c3->SaveAs("AttachmentCoefficient.png");

  TCanvas* c4 = new TCanvas("c4", "Longitudinal Diffusion", 800, 600);
  TGraph* grLongDiff = new TGraph(nE, &efields[0], &longitudinalDiffusions[0]);
  grLongDiff->SetTitle("Longitudinal Diffusion;Electric Field [V/cm];Diffusion [√cm]");
  grLongDiff->SetLineColor(kMagenta);
  grLongDiff->SetLineWidth(2);
  grLongDiff->Draw("AL");
  c4->SaveAs("LongitudinalDiffusion.png");

  TCanvas* c5 = new TCanvas("c5", "Transverse Diffusion", 800, 600);
  TGraph* grTransDiff = new TGraph(nE, &efields[0], &transverseDiffusions[0]);
  grTransDiff->SetTitle("Transverse Diffusion;Electric Field [V/cm];Diffusion [√cm]");
  grTransDiff->SetLineColor(kOrange);
  grTransDiff->SetLineWidth(2);
  grTransDiff->Draw("AL");
  c5->SaveAs("TransverseDiffusion.png");

  // Save data to CSV file
  std::ofstream outFile("transport_parameters.csv");
  outFile << "E_Field[V/cm],Drift_Velocity[cm/us],Townsend_Coeff[1/cm],Attachment_Coeff[1/cm],Longitudinal_Diffusion[√cm],Transverse_Diffusion[√cm]\n";
  for (size_t i = 0; i < nE; ++i) {
    outFile << efields[i] << "," << driftVelocities[i] << "," << townsendCoeffs[i] << "," << attachmentCoeffs[i] << "," << longitudinalDiffusions[i] << "," << transverseDiffusions[i] << "\n";
  }
  outFile.close();

  app.Run();

  return 0;
}

