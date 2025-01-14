# Transport-without-B
this is Garfield++ program that simulates 5 Transport Parameter without Magnetic Field 
plot and save results to transport_parameters.csv file
Townsend Coefficient
Attachment Coefficient
Longitudinal Diffusion
Transverse Diffusion
Drift Velocity
note:all the transport including E are in linear 
this simple program make the output of the work of Yasser Assran, Archana Sharma
Transport Properties of operational gas mixtures used at LHC
`//If you want the B magnetic field and the angle to be included in the simulations 
for (size_t j = 0; j < nB; ++j) {
  for (size_t k = 0; k < nA; ++k) {
    std::cout << "B = " << bfields[j] << " T, theta = "
              << angles[k] * RadToDegree << " degree\n";
    std::cout << "   E [V/cm]     vE [cm/us]    alpha [1/cm]\n";
    for (size_t i = 0; i < nE; ++i) {
       double ve = 0.;
       gas.GetElectronVelocityE(i, j, k, ve);
       // Convert from cm/ns to cm/us.
       ve *= 1.e3;
       double alpha = 0.;
       gas.GetElectronTownsend(i, j, k, alpha);
       alpha = exp(alpha);
       std::printf("%10.3f    %10.3f    %10.3f\n", efields[i], ve, alpha);
    }
  }
}





`
