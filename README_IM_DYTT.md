# EMEP MSC-W ACTM rv5.0_IM: adaptation of rv5.0 to include HCl chemistry, assuming internally mixed fine sea salt

Changes made by Damaris Y. T. Tan (damtan@ceh.ac.uk / damaris.tan@ed.ac.uk)
UK Centre for Ecology & Hydrology / University of Edinburgh 
Published on Zenodo November 2026.
Paper describing model changes to be submitted to ACP shortly.

Changes in the code are marked by the author's initials (DYTT). 
The initials may be hyphenated with an additional letter combination,
e.g. ''DYTT-SSfreac''.
Please ignore the term after the hyphen - this was part of the model 
adaptation process and is no-longer relevant.

Original model code (Simpson et al., 2012; Fagerli et al., 2024) is available 
from the Norwegian Meteorological Institute GitHub page: 
https://github.com/metno/emep-ctm, and should be credited accordingly. 

## Overview

HCl chemistry has been added, assuming internal mixing of fine sea salt. 

HCl is produced via direct emissions, and via chemical reaction:
HNO3 + coarse sea salt -> HCl + coarse NaNO3 (this is a pre-existing reaction, 
but HCl production was previously not accounted for.)
The HNO3 + fine sea salt -> HCl + fine NaNO3 reaction is accounted for in the 
ISORROPIA / EQSAM equilibrium modules. 

HCl equilibrium with NH3 is also accounted for via the ISORROPIA / EQSAM 
equilibrium module. 

HCl is removed through wet and dry deposition. 

Individual fine sea salt ions are now treated stoichiometrically, i.e. they 
are tracked. This was not previously the case.

## Definitions of new species

8 new species have been added: 

HCl gas.
pCl: fine particulate chloride. This is both fine chloride associated
     with NH4Cl and fine chloride associated with fine sea salt. By 
     definition of internal mixing, both forms of chloride exist in the 
     same aerosol particle and are indistinguishable. 
pNa: fine particulate sodium associated with sea salt.
pMg: fine particulate magnesium associated with sea salt.
pCa: fine particulate calcium associated with sea salt.
pK: fine particulate potassium associated with sea salt.
pSO4: fine particulate sulfate associated with sea salt.
pRem: the remaining fine particulate component of sea salt, assumed to be 
      mostly bromide.

The SEASALT_F species only exists for the production step. After this, it is 
replaced by its individual ionic components (pCl, pNa, pMg etc.). 

A note on fine nitrate, NO3_f:
The definition of this species has changed slightly: it is now the fine nitrate
associated with both NH4NO3 and NaNO3. By definition of internal mixing, these
exist in the same aerosol particle and are indistinguishable. 

## Summary of changes in individual modules

### AeroConstants_mod.f90

This code only works with the ISORROPIA and EQSAM equilibrium modules. 
It does not work with MARS (which has no functionality for treating HCl).
EQSAM has better agreement with measurements, though this strongly 
depends on the choice of uptake coefficient (GAM) for the reaction 
between coarse sea salt and HNO3 to form HCl (defined in CM_ChemRates_mod.f90). 
For more details, see the paper to be submitted shortly. 
ISORROPIA may work better if paired with a different uptake coefficient. 

The INTERNALMIXED flag is set to .true., because this is the assumption 
made by this model version. This flag is not, however, used in the 
code. Setting this to .false. will change nothing.

### AeroFunctions.f90

Removed any mention of SSf, because this variable is no-longer used. 

### AerosolCalls.f90

By definition, this code works on the assumption of internally mixed 
fine sea salt, i.e. AERO%INTERNALMIXED = T. Hence all possibilities for
externally mixed sea salt are commented out.

All instances of fine sea salt (SSf) have been replaced by the individual 
ionic components of sea salt (pCl, pNa, pK etc.).

Note that the AERO%CATIONS flag determines if pCa, pK and pMg are input 
into ISORROPIA. This is the approach taken by the original model code rv5.0.
Use AERO%CATIONS = F with caution, as I see no situation in which sea salt 
anions are input into ISORROPIA, but not the sea salt cations.

### BoundaryConditions_mod.f90

Added in boundary conditions for fine sea salt individual ionic components
(pCl, pNa, pCa etc.), to replace the boundary conditions for SeaSalt_f.

### CloudJ_mod.f90

Replaced fine sea salt with its individual ionic components (pCl, pNa, pCa etc.).
Need to make assumption that pCl is predominantly sea salt. 
It is assumed that fine sea salt deposits like all other fine PM - a sensible
assumption, since fine sea salt is internally mixed. 

### CM_ChemDims_mod.f90

Changed array dimensions to account for the addition of new species and
emissions.

### CM_ChemGroups_mod.f90

*** Bugfix: WDEP_PMFINE_GROUP and DDEP_PMFINE_GROUP did not contain OM25_p. 
Have added this. If there is a good reason why OM25_p was included in 
PMFINE_GROUP but not the WDEP and DDEP equivalent, I do not know it but 
would like to find out. ***

Replaced SeaSalt_f with pCl, pNa, pMg, pCa, pK, pSO4 and pRem in various
groups. 

Added pSO4 to the SOX group. Proceed with caution when using SOX. There 
will be cases when you do not want to include the sea salt component of 
SOX, i.e. pSO4. 

### CM_ChemRates_mod.f90

Changed the uptake coefficient of HNO3 onto a coarse sea salt particle 
(GAM) from 0.01 to 0.04. For reasoning behind this choice, see the paper 
to be submitted.

### CM_ChemSpecs_mod.f90

Added HCl, pCl, pNa, pMg, pCa, pK, pSO4 and pRem species. 
Removed fine sea salt where possible.

### CM_DryDep.inc

HCl deposits like HCl (i.e. same properties as HNO3, but with diffusivity 
in air specific to HCl).
Removed fine sea salt. 
pNa, pMg, pCa, pK, pSO4 and pRem all are assumed to be internally mixed and
deposit like all other fine PM. 
The fraction of pCl associated with NH4Cl deposits like NH4NO3 (i.e. accounting
for the near-surface evaporation effect. The rest is assumed to be associated
with fine sea salt which is internally mixed, and deposits like all other 
fine PM.)

### CM_EmisFile.inc

Added the possibility for HCl and pCl emission files.

### CM_EmisSpecs.inc

Added the possibility for HCl and pCl emissions.

### CM_Reactions1.inc

Added HCl production via direct emissions, and reaction between HNO3 and coarse 
sea salt. Reaction between HNO3 and fine sea salt is accounted for in ISORROPIA/
EQSAM. 
Added pCl production via direct emissions, and from fine sea salt production. 
Added pNa, pMg, pCa, pK, pSO4 and pRem production from fine sea salt production.

### CM_Reactions2.inc

Removed production of fine sea salt, as this is accounted for in the production 
of pCl, pNa, pMg, pCa, pK, pSO4 and pRem, such that fine sea salt mass is conserved.

### CM_WetDep.inc

HCl wet deposition is like HNO3, following approach by Wang et al. (2025).

Removed fine sea salt, replacing it with pCl, pNa, pMg, pCa, pK, pSO4 and pRem,
which all deposit like fine PM, as these are assumed to be internally mixed,
and therefore deposit at the same rate as all other fine PM. 

### CMX_BoundaryConditions.txt

Replaced fine sea salt with pCl, pNa, pMg, pCa, pK, pSO4 and pRem. 
Assumption is that most pCl is from sea salt. 

### config_emep.nml

Removed SEASALT_F.

### Config_module.f90

*** Changed the end of the day to 00:00 to correspond with UK measurements.
This may need to be changed back to 06:00 to correspond with EMEP measurements.***

Added in HCl, pCl, pNa, pMg, pCa, pK, pSO4 and pRem where required. 

### DryDep_mod.f90

Amended this to ensure that both NH4NO3 and NH4Cl deposit in the same way, 
i.e. accounting for the near-surface evaporation effect. 
Required making sure the correct fraction of NH4 deposits in this way, i.e. only 
the NH4 associated with NH4NO3 and NH4Cl.
Also required making sure that only the Cl associated with NH4Cl deposits 
this way, but the Cl associated with fine sea salt does not. 

### EQSAM_mod.f90

Added HCl, pNa, pMg, pCa and pK as an input. EQSAM has pre-existing capability
for dealing with these. 
Had to make sure that order of subroutine arguments corresponds to the order 
in AerosolCalls.f90!!!

### ForestFire_mod.f90

Bugfix unrelated to HCl chemistry or sea salt mixing:
The old forest fire dispersion method (P800) was being printed regardless of 
which method is actually being used. 
The code has been ammended to print the dispersion method matching the one being 
used by the model.

### GasParticleCoeffs_mod.f90

Added gas properties for HCl - same as for HNO3 (Wang et al., 2025)
but using a diffusivity specific to HCl of 0.16 cm2/s at STP, which is 
the literature value quoted in Meyer (2023).

Added DDdef for PMfCl, which is the same as for PMfNO3, but applied to 
only the fraction of Cl associated with NH4Cl (see DryDep_mod.f90).

### Setup_1d_mod.f90

Removed fine sea salt.

## References

Fagerli, H., Benedictow, A., Blake, L., van Caspel, W., Denby, B. R., Gauss, M., Jonson, J. E., Klein, H., Lange, G. F., Mousing, E. A., Nyíri, A., Olivié, D., Segers, A., Simpson, D., Tsyro, S., Valdebenito, A., Wind, P., Aas, W., Fiebig, M., Hjellbrekke, A., Solberg, S., Tørseth, K., Yttri, K. E., Redeyoff, O., Matthews, B., Schindlbacher, S., Ullrich, B., Wankmüller, R., Scheuschner, T., Kuenen, J. P., Guevara, M., l Jaffrezo, J., Dominutti, P., Uzu, G., Conil, S., Favez, O., and Moˇcnik, G.: Convention on Long-range Transboundary Air Pollution Co-operative programme for monitoring and evaluation of the lang-range transmission of air pollutants in Europe, Norwegian Meteorological Institute, https://emep.int/publ/reports/2024/EMEP_Status_Report_1_2024.pdf, 2024.

Meyer, P.: Quantitative Determination of the Diffusion Constant of Hydrogen Chloride in Air: A Way to Eliminate a (School-Based) Misconception and to Re-evaluate the Counterdiffusion of HCl and NH3, Journal of Chemical Education, 100, 4941–4945, https://doi.org/10.1021/acs.jchemed.3c00778, 2023.

Simpson, D., Benedictow, A., Berge, H., Bergström, R., Emberson, L. D., Fagerli, H., Flechard, C. R., Hayman, G. D., Gauss, M., Jonson,J. E., Jenkin, M. E., Nyíri, A., Richter, C., Semeena, V. S., Tsyro, S., Tuovinen, J.-P., Valdebenito, A., and Wind, P.: The EMEP MSC-W chemical transport model - technical description, Atmospheric Chemistry and Physics, 12, 7825–7865, https://doi.org/10.5194/acp-12-7825-2012, 2012.

Wang, Y., Nemitz, E., Tomlinson, S. J., Carnell, E. J., Yao, L., Scheffler, J., Liska, T., Pearson, C., Dragosits, U., Venkataraman, C., Balasubramanian, S., Beck, R., Sutton, M. A., and Vieno, M.: Response of South Asia PM2.5 pollution to ammonia emission changes and associated impacts on human health, Environment International, 195, 109207, https://doi.org/10.1016/j.envint.2024.109207, 2025.


