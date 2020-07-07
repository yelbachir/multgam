#include "../inst/include/Family.hpp"
#include "../inst/include/FamilyExpon.hpp"
#include "../inst/include/FamilyBinomial.hpp"
#include "../inst/include/FamilyGamma.hpp"
#include "../inst/include/FamilyPoiss.hpp"
#include "../inst/include/FamilyAngLogit.hpp"
#include "../inst/include/FamilyGauss.hpp"
#include "../inst/include/FamilyGev.hpp"
#include "../inst/include/FamilyGpd.hpp"
#include "../inst/include/FamilyPp.hpp"
#include "../inst/include/FamilyRgev.hpp"


// global vars
const std::array<StructFamily, nbFamily> familyChoice = {{
  {logLikGev, Deriv1iGev, Deriv2iGev, Deriv3iGev, betaInitGev},
  {logLikGauss, Deriv1iGauss, Deriv2iGauss, Deriv3iGauss, betaInitGauss},
  {logLikAngLogit, Deriv1iAngLogit, Deriv2iAngLogit, Deriv3iAngLogit, betaInitAngLogit},
  {logLikPoiss, Deriv1iPoiss, Deriv2iPoiss, Deriv3iPoiss, betaInitPoiss},
  {logLikGamma, Deriv1iGamma, Deriv2iGamma, Deriv3iGamma, betaInitGamma}, 
  {logLikBinom, Deriv1iBinom, Deriv2iBinom, Deriv3iBinom, betaInitBinom},
  {logLikExpon, Deriv1iExpon, Deriv2iExpon, Deriv3iExpon, betaInitExpon},
  {logLikGpd, Deriv1iGpd, Deriv2iGpd, Deriv3iGpd, betaInitGpd},
  {logLikPp, Deriv1iPp, Deriv2iPp, Deriv3iPp, betaInitPp},
  {logLikRgev, Deriv1iRgev, Deriv2iRgev, Deriv3iRgev, betaInitRgev}

}};
