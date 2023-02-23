#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TStyle.h"
#include "TDirectory.h"
#include "TString.h"

Double_t normStud( Double_t *x, Double_t *par5 ) {
  static int nn = 0;
  nn++;
  static double dx = 0.1;
  static double b1 = 0;
  if( nn == 1 ) b1 = x[0];
  if( nn == 2 ) dx = x[0] - b1;
  //--  Mean and width:
  double xm = par5[0];
  double t = ( x[0] - xm ) / par5[1];
  double tt = t*t;
  //--  exponent:
  double rn = par5[2];
  double xn = 0.5 * ( rn + 1.0 );
  //--  Normalization needs Gamma function:
  double pk = 0.0;
  if( rn > 0.0 ) {
    double pi = 3.14159265358979323846;
    double aa = dx / par5[1] / sqrt(rn*pi) * TMath::Gamma(xn) / TMath::Gamma(0.5*rn);
    pk = par5[3] * aa * exp( -xn * log( 1.0 + tt/rn ) );
  }
  return pk + par5[4];
};


//----------------------------------------------------------------------
//

int fittp0(const char* hs , float & sigma_res,float & sigma_res_err , float & mean_res,float & mean_res_err,  TString HV_val, TString Run, string outdir,string fitType, string region, TString file_name) {
  TCanvas* c = new TCanvas("c","c",0,0,800,800); 
  c->Draw();
  const char* fit;
  gROOT->Reset();
  gStyle->SetOptStat(0000);

  c->SetLeftMargin(0.15);
 
  TPad *pad = new TPad("pad","pad",0.2,0.01,0.99,0.99);
  pad->SetBottomMargin(0.18);
  pad->SetLeftMargin(0.15);
  cout <<" Created pad for hist"<< hs  <<endl;
  
  TH1 *h = (TH1*)gDirectory->Get(hs);

  if( h == NULL ){
    cout << hs << " does not exist\n";
  }
  else{
    h->SetMarkerStyle(21);
    h->SetMarkerSize(0.8);
     
    h->SetTitle("");
    h->Rebin(40);
    TGaxis::SetMaxDigits(3);
    double dx = h->GetBinWidth(1);
    double nmax = h->GetBinContent(h->GetMaximumBin());
    double xmax = h->GetBinCenter(h->GetMaximumBin());
    double nn = 7*nmax;
    int nb = h->GetNbinsX();
    double n1 = h->GetBinContent(1);
    double n9 = h->GetBinContent(nb);
    double bg = 0.5*(n1+n9);
    double x1 = h->GetBinCenter(1);
    double x9 = h->GetBinCenter(nb);

    TString name =  (TString) hs ;
    // create a TF1 with the range from x1 to x9 and 5 parameters
   
    TF1 *tp0Fcn ;
    TF1 *tp0Fcn_gaus ;

    cout << "student t fit" << endl;
    tp0Fcn = new TF1( "tp0Fcn", normStud, x1, x9, 5 );
    fit = "Student-t fit";
    tp0Fcn->SetParName( 0, "mean" );
    tp0Fcn->SetParName( 1, "sigma" );
    tp0Fcn->SetParName( 2, "nu" );
    tp0Fcn->SetParName( 3, "area" );
    tp0Fcn->SetParName( 4, "BG" );
    
    tp0Fcn->SetNpx(500);
    tp0Fcn->SetLineWidth(4);
    tp0Fcn->SetLineColor(kGreen);
    tp0Fcn->SetLineColor(kGreen);
    
    // set start values for some parameters:
    
    tp0Fcn->SetParameter( 0, 0.);//xmax ); // peak position
    tp0Fcn->SetParameter( 1, 4*dx ); // width
    tp0Fcn->SetParameter( 2, 2.2 ); // nu
    tp0Fcn->SetParameter( 3, nn ); // N
    tp0Fcn->SetParameter( 4, bg );
    h->Fit( "tp0Fcn", "R", "ep" );

    sigma_res= tp0Fcn->GetParameter(1);
    sigma_res_err= tp0Fcn->GetParError(1);
    mean_res= tp0Fcn->GetParameter(0);
    mean_res_err= tp0Fcn->GetParError(0);

    std::cout << "=== FIT RESULT === " <<  hs << ": mean " <<mean_res << " +-"  << mean_res_err   << " sigma "<< sigma_res << " +-"  << sigma_res_err  <<std::endl;
    std::cout << "*************"<<std::endl;

    if(mean_res_err!=mean_res_err){
      mean_res_err=100.;};
    if(sigma_res_err!=sigma_res_err){
      sigma_res_err=100.;};
    //TLegend *pl2 = new TLegend(0.55,0.54,0.65,0.89);
    TLegend *pl2 = new TLegend(0.55,0.84,0.65,0.89);
    pl2->SetTextSize(0.035); 
    pl2->SetFillColor(0);
    pl2->SetBorderSize(0);


    TLegend *pl3 = new TLegend(0.6,0.6,0.65,0.65);
    pl3->SetTextSize(0.035); 
    pl3->SetFillColor(0);
    pl3->SetBorderSize(0);

    std::ofstream outfile;
    TString name_of_text_file = (TString)outdir+"/"+(TString)"Param_"+(TString)fitType+"_"+Run+(TString)".txt";
    outfile.open(name_of_text_file.Data(), std::ios_base::app);
    cout << "========>" << name << endl;


    if(region=="BPix"){
    if(strstr(name, "h520") != NULL)   outfile << "L1, x, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "h420") != NULL)   outfile << "L2, x, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "hg420") != NULL)   outfile << "L3, x, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "g520") != NULL)   outfile << "L4, x, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "h521") != NULL)   outfile << "L1, z, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "h421") != NULL)   outfile << "L2, z, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "hg421") != NULL)   outfile << "L3, z, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "g521") != NULL)   outfile << "L4, z, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    };
    if(region=="FPix"){
    if(strstr(name, "h420f2") != NULL)   outfile << "D1, x, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "h420f3") != NULL)   outfile << "D2, x, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "h420f4") != NULL)   outfile << "D3, x, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "h421f2") != NULL)   outfile << "D1, z, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "h421f3") != NULL)   outfile << "D2, z, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    if(strstr(name, "h421f4") != NULL)   outfile << "D3, z, sigma, delta_sigma, mu, delta_mu, RMS, delta_RMS, " << HV_val << ": ";
    };

    TString histname;
    if(region=="BPix"){
    if(name.Contains("h520"))  histname = "L1_x";
    if(name.Contains("h420"))  histname = "L2_x";
    if(name.Contains("hg420")) histname = "L3_x";
    if(name.Contains("g520"))  histname = "L4_x";
    if(name.Contains("h521"))  histname = "L1_z";
    if(name.Contains("h421"))  histname = "L2_z";
    if(name.Contains("hg421")) histname = "L3_z";
    if(name.Contains("g521"))  histname = "L4_z";
    };
    if(region=="FPix"){
    if(name.Contains("h420f2"))   histname = "D1_x";
    if(name.Contains("h420f3"))   histname = "D2_x";
    if(name.Contains("h420f4"))   histname = "D3_x";
    if(name.Contains("h421f2"))   histname = "D1_z";
    if(name.Contains("h421f3"))   histname = "D2_z";
    if(name.Contains("h421f4"))   histname = "D3_z";
    };

    outfile << sigma_res << ", " << sigma_res_err << ", " << mean_res << ", " << mean_res_err << ", " << h->GetRMS() <<  ", " << h->GetRMSError() << std::endl;
    if(region=="FPix"){
    if ( name.Contains("f2"))  pl2->SetHeader("Forward Pixel Disk 1");
    if ( name.Contains("f3"))  pl2->SetHeader("Forward Pixel Disk 2");
    if ( name.Contains("f4"))  pl2->SetHeader("Forward Pixel Disk 3");
    };
    if(region=="BPix"){
    if ( name.Contains("h52") )  pl2->SetHeader("Barrel Pixel Layer 1");
    if ( name.Contains("h42") || name.Contains("hf42") )  pl2->SetHeader("Barrel Pixel Layer 2");
    if ( name.Contains("hg42") || name.Contains("i52") )  pl2->SetHeader("Barrel Pixel Layer 3");
    if ( name.Contains("f52") || name.Contains("g52") )  pl2->SetHeader("Barrel Pixel Layer 4");
    };

    TLegendEntry *ple3 = pl3->AddEntry((TObject*)0, "#sigma_{r}="+TString::Format("%.2f",sigma_res )+"#pm"+ TString::Format("%.2f",sigma_res_err )  +" #mum",  "");
    
    pl2->Draw("same");
    pl3->Draw("same");

    h->GetYaxis()->SetTitle("Number of hits / "+ TString::Format("%.0f",dx) +" #mum");
    h->GetXaxis()->SetTitle("Residuals r-#phi direction (#mum)");
    if ( name.Contains("21"))  h->GetXaxis()->SetTitle("Residuals z direction (#mum)");

    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleOffset(1.4); 
    h->GetXaxis()->SetTitleOffset(0.9);
    h->GetXaxis()->SetLabelSize(0.045);
    h->GetYaxis()->SetLabelSize(0.045);
    //h->GetXaxis()->SetRangeUser(-10,10);

    TString cmsText     = "CMS";
    float cmsTextFont   = 61;  // default is helvetic-bold
    bool writeExtraText = true;
    TString extraText   = "Work in progress"; 
    float extraTextFont = 52;  // default is helvetica-italics
    // text sizes and text offsets with respect to the top frame in unit of the top margin size
    float lumiTextSize     = 0.5;
    float lumiTextOffset   = 0.15;
    float cmsTextSize      = 0.65;
    float cmsTextOffset    = 0.1;  // only used in outOfFrame version
    float relPosX    = 0.045;
    float relPosY    = 0.035;
    float relExtraDY = 1.2;
    // ratio of "CMS" and extra text size
    float extraOverCmsTextSize  = 0.65;
    TString lumiText;
    lumiText = "(2022) 13.6 TeV ";
    float t = pad->GetTopMargin();
    float b = pad->GetBottomMargin();
    float r = pad->GetRightMargin();
    float l = pad->GetLeftMargin();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextAngle(0);
    latex.SetTextColor(kBlack);    
    float extraTextSize = extraOverCmsTextSize*cmsTextSize;
    latex.SetTextFont(42);
    latex.SetTextAlign(31); 
    latex.SetTextSize(lumiTextSize*t);    
    latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);
    latex.SetTextFont(cmsTextFont);
    latex.SetTextAlign(11); 
    latex.SetTextSize(cmsTextSize*t);
    latex.DrawLatex(l+0.03,1-t+lumiTextOffset*t-0.09,cmsText);   
    latex.SetTextFont(extraTextFont);
    latex.SetTextSize(extraTextSize*t);
    latex.DrawLatex(l+0.03, 1-t+lumiTextOffset*t-0.09-0.06, extraText);    
    TString TrackRecoText = (TString)fitType+" reconstruction";
    latex.SetTextFont(42);
    latex.SetTextSize(extraTextSize*t*0.95);
    latex.DrawLatex(l+0.03, 1-t+lumiTextOffset*t-0.09-0.06-0.06, TrackRecoText);      
    TString TrackSelectionText;
    if(region=="FPix") TrackSelectionText = "Track p_{T} > 4 GeV";
    if(region=="BPix") TrackSelectionText = "Track p_{T} > 12 GeV";
    latex.SetTextFont(42);
    latex.SetTextSize(extraTextSize*t*0.95);
    latex.DrawLatex(l+0.03, 1-t+lumiTextOffset*t-0.09-0.06-0.12, TrackSelectionText);        
    TString RunText = "Run: " + (TString) Run;
    latex.SetTextFont(42);
    latex.SetTextSize(extraTextSize*t*0.75);
    latex.DrawLatex(l+0.03, 1-t+lumiTextOffset*t-0.09-0.06-0.18, RunText);


    h->Draw("hist ep same"); // data again on top
    gPad->RedrawAxis("same");
    c->Update();
    h->SetMaximum(tp0Fcn->GetMaximum() * 1.7);
    h->Draw("hist ep same");

    //c->Update();
   
    //name +=(TString) "_"+Run+(TString)"_" + HV_val+(TString)"_"+(TString)fitType+".pdf" ;
    TString filename = histname+(TString) "_"+Run+(TString)"_" + HV_val+(TString)"_"+(TString)fitType+".pdf" ;
    //TString name_png = (TString) hs ;
    TString filename_png = histname+(TString) "_"+Run+(TString)"_" + HV_val +  (TString)"_"+(TString)fitType+".png" ;
    
    c->SaveAs((TString)outdir+(TString)"/"+filename);
    c->SaveAs((TString)outdir+(TString)"/"+filename_png);
    //delete tp0Fcn;

  }
 return 0;
}


void FitAndPlot(const char* file, string dir, string run, string outFolder, string fitType, string region, string file_name){
  TFile *_file0 = new TFile(file);
  TString Run;
  TString HV_val;
  TString name;
  //TString Reco;
  TString layerHV;
 
  Run = run;
  name = file_name;

  TString directory = dir;
  _file0->cd(directory+Run);

  if(region=="FPix"){
  //Disk1
  //Get resolution along x
  TH1F*  h420f2_123= (TH1F*) _file0->Get(directory+Run+(TString)"/h420f2_123");
  h420f2_123->SetDirectory(0); // "detach" the histogram from the file
  float sigma_x,sigma_x_err,mean_x,mean_x_err ;
  fittp0("h420f2_123",sigma_x,sigma_x_err,mean_x,mean_x_err, HV_val, Run, outFolder, fitType, region, name);
  
  //Get resolution along z
  TH1F*  h421f2_123= (TH1F*) _file0 ->Get(directory+Run+(TString)"/h421f2_123");
  h421f2_123->SetDirectory(0); // "detach" the histogram from the file
  float sigma_z,sigma_z_err, mean_z, mean_z_err ;
  fittp0("h421f2_123", sigma_z, sigma_z_err, mean_z, mean_z_err, HV_val, Run, outFolder, fitType, region, name);
  
  //Disk2
  //Get resolution along x
  TH1F*  h420f3_234= (TH1F*) _file0->Get(directory+Run+(TString)"/h420f3_234");
  h420f3_234->SetDirectory(0); // "detach" the histogram from the file
  float sigma_x_l142,sigma_x_err_l142,mean_x_l142,mean_x_err_l142 ;
  fittp0("h420f3_234",sigma_x_l142,sigma_x_err_l142,mean_x_l142,mean_x_err_l142, HV_val, Run, outFolder, fitType, region, name);
  
  //Get resolution along z
  TH1F*  h421f3_234= (TH1F*) _file0 ->Get(directory+Run+(TString)"/h421f3_234");
  h421f3_234->SetDirectory(0); // "detach" the histogram from the file
  float sigma_z_l142,sigma_z_err_l142, mean_z_l142, mean_z_err_l142 ;
  fittp0("h421f3_234", sigma_z_l142, sigma_z_err_l142, mean_z_l142, mean_z_err_l142, HV_val, Run, outFolder, fitType, region, name);
  
  //Disk3
  //Get resolution along x
  TH1F*  h420f4_234= (TH1F*) _file0->Get(directory+Run+(TString)"/h420f4_234");
  h420f4_234->SetDirectory(0); // "detach" the histogram from the file
  float sigma_x_l124,sigma_x_err_l124,mean_x_l124,mean_x_err_l124 ;
  fittp0("h420f4_234",sigma_x_l124,sigma_x_err_l124,mean_x_l124,mean_x_err_l124, HV_val, Run, outFolder, fitType, region, name);
  
  //Get resolution along z
  TH1F*  h421f4_234= (TH1F*) _file0 ->Get(directory+Run+(TString)"/h421f4_234");
  h421f4_234->SetDirectory(0); // "detach" the histogram from the file
  float sigma_z_l124,sigma_z_err_l124, mean_z_l124, mean_z_err_l124 ;
  fittp0("h421f4_234", sigma_z_l124, sigma_z_err_l124, mean_z_l124, mean_z_err_l124, HV_val, Run, outFolder, fitType, region, name);

  _file0->Close();
    };
  
  if(region=="BPix"){
  //Layer1 
  //Get resolution along x
  TH1F*  h520= (TH1F*) _file0->Get(directory+Run+(TString)"/h520");
  h520->SetDirectory(0); // "detach" the histogram from the file
  float sigma_x_l231,sigma_x_err_l231,mean_x_l231,mean_x_err_l231 ;
  fittp0("h520",sigma_x_l231,sigma_x_err_l231,mean_x_l231,mean_x_err_l231, HV_val, Run, outFolder, fitType, region, name);
 
  //Get resolution along z
  TH1F*  h521= (TH1F*) _file0 ->Get(directory+Run+(TString)"/h521");
  h521->SetDirectory(0); // "detach" the histogram from the file
  float sigma_z_l231,sigma_z_err_l231, mean_z_l231, mean_z_err_l231 ;
  fittp0("h521", sigma_z_l231, sigma_z_err_l231, mean_z_l231, mean_z_err_l231, HV_val, Run, outFolder, fitType, region, name);

  //Layer2 
  //Get resolution along x
  TH1F*  h420= (TH1F*) _file0->Get(directory+Run+(TString)"/h420");
  h420->SetDirectory(0); // "detach" the histogram from the file
  float sigma_x,sigma_x_err,mean_x,mean_x_err ;
  fittp0("h420",sigma_x,sigma_x_err,mean_x,mean_x_err, HV_val, Run, outFolder, fitType, region, name);
 
  //Get resolution along z
  TH1F*  h421= (TH1F*) _file0 ->Get(directory+Run+(TString)"/h421");
  h421->SetDirectory(0); // "detach" the histogram from the file
  float sigma_z,sigma_z_err, mean_z, mean_z_err ;
  fittp0("h421", sigma_z, sigma_z_err, mean_z, mean_z_err, HV_val, Run, outFolder, fitType, region, name);

  //Layer3 
  //Get resolution along x
  TH1F*  hg420= (TH1F*) _file0->Get(directory+Run+(TString)"/hg420");
  hg420->SetDirectory(0); // "detach" the histogram from the file
  float sigma_x_l243,sigma_x_err_l243,mean_x_l243,mean_x_err_l243 ;
  fittp0("hg420",sigma_x_l243,sigma_x_err_l243,mean_x_l243,mean_x_err_l243, HV_val, Run, outFolder, fitType, region, name);
 
  //Get resolution along z
  TH1F*  hg421= (TH1F*) _file0 ->Get(directory+Run+(TString)"/hg421");
  hg421->SetDirectory(0); // "detach" the histogram from the file
  float sigma_z_l243,sigma_z_err_l243, mean_z_l243, mean_z_err_l243 ;
  fittp0("hg421", sigma_z_l243, sigma_z_err_l243, mean_z_l243, mean_z_err_l243, HV_val, Run, outFolder, fitType, region, name);

  //Layer4 
  //Get resolution along x
  TH1F*  g520= (TH1F*) _file0->Get(directory+Run+(TString)"/g520");
  g520->SetDirectory(0); // "detach" the histogram from the file
  float sigma_x_l234,sigma_x_err_l234,mean_x_l234,mean_x_err_l234 ;
  fittp0("g520",sigma_x_l234,sigma_x_err_l234,mean_x_l234,mean_x_err_l234, HV_val, Run, outFolder, fitType, region, name);
 
  //Get resolution along z
  TH1F*  g521= (TH1F*) _file0 ->Get(directory+Run+(TString)"/g521");
  g521->SetDirectory(0); // "detach" the histogram from the file
  float sigma_z_l234,sigma_z_err_l234, mean_z_l234, mean_z_err_l234 ;
  fittp0("g521", sigma_z_l234, sigma_z_err_l234, mean_z_l234, mean_z_err_l234, HV_val, Run, outFolder, fitType, region, name);
 

  _file0->Close();
  };
}
