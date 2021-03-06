# multgam: automatic smoothing for multiple GAMs
The Rcpp package `multgam` implements the empirical Bayes optimization algorithm described in El-Bachir and Davison (2019), which trains multiple generalized additive models (GAMs) and automatically tunes their L2 regularization hyper-parameters. Moreover, `multgam` provides automatic ridge penalty for multiple parametric non-linear regression models, where the linear or non-linear functions of inputs are not necessarily smooth but their regression weights are constrained by the L2 penalty with possibly different hyper-parameters.

The package `multgam` uses R as an interface for the optimization code implemented in C++, and uses the R package `mgcv` to set up the matrix of inputs, to visualize the learned functions, and to perform predictions.

## Table of contents
[1. Installation](#install)                                     
[2. Usage](#usage)                  
    &nbsp;&nbsp;&nbsp;&nbsp;[2.1. Main training function](#mainFunc)   
    &nbsp;&nbsp;&nbsp;&nbsp;[2.2. Supported probability distributions and examples](#supportedDistrib)          
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[2.2.1. Classical exponential family distributions](#classical)         
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[2.2.2. Extreme value distribution families](#evd)          
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[2.2.3. Examples](#examples)          
    &nbsp;&nbsp;&nbsp;&nbsp;[2.3. Extension to new distributions](#newDistrib)          
[3. General comments](#comments)          
[4. Bugs, help and suggestions](#bugs)          
[5. Citation](#citation)

<a name="install"/></a>
## 1. Installation
The Rcpp package `multgam` must be installed from source as follows.
- Download and extract the directory `multgam`, which contains the package.
- In the R file `install.R`, update the character variable `path2multgam` with the path to `multgam`. For example, if `multgam` has been extracted in your desktop and you are using linux, you could use 
```R
path2multgam <- "~/Desktop/multgam"
```
- Run the file `install.R`.

<a name="usage"/></a>
## 2. Usage

The output/response variable can be a vector or a matrix from a univariate or a multivariate probability distribution, but the log-likelihood for the full dataset must be expressed as the sum of the log-likelihoods for an individual observation. A particular case is independent random observations. 

In practice, `multgam` interprets a GAM as a multiple linear regression model whose weights are subject to the L2 penalty, and computes the corresponding regularization matrices. When the functions of inputs are smooth, splines for example, the regularization matrices are dense and represent the smoothing matrices. When the functions of inputs are weighted sums of predictors, the regularization matrices are the identity matrices, to which the user can assign different regularization hyper-parameters; see the argument `groupReg` in the function `mtgam` in [Section 2.1](#mainFunc). 

<a name="mainFunc"/></a>
### 2.1. Main training function

Train a multiple generalized additive model using the function `mtgam` as follows
```R
fit <- mtgam(dat, L.formula, fmName="gauss", lambInit=NULL, betaInit=NULL, groupReg=NULL, 
             iterMax=200, progressPen=FALSE, PenTol=.Machine$double.eps^.5, progressML=FALSE, MLTol=1e-07, ...)
``` 
with **arguments**:
- `dat`: a list or a data frame whose columns contain the input and the output variables used in `L.formula`; specific distribution family considerations can be found in [Section 2.2](#supportedDistrib),
- `L.formula`: a list of as many formulas as there are output variables with additive structures of input variables. For additional information on `dat` and `L.formula` see the examples in [Section 2.2](#supportedDistrib), or the documentation of the R package `mgcv` on CRAN. The package `multgam` has been tested with the basis functions `bs="tp"` (thin plate regression splines), `bs="cr"` (cubic regression splines), `bs="cc"` (cyclic cubic regression splines),
- `fmName`: a character variable for the name of the probability distribution of the output variables: `"gauss"` for the Gaussian distribution, `"poisson"` for the Poisson distribution, `"binom"` for the binomial distribution, `"expon"` for the exponential distribution, `"gamma"` for the gamma distribution, `"gev"` for the generalized extreme value distribution, `"gpd"` for the generalized Pareto distribution, `"pp"` for the point process approach in extreme value analysis, `"rgev"` for the r-largest extreme value distribution. Details on their parametrization and specific considerations can be found in [Section 2.2](#supportedDistrib),
- `lambInit`: a vector of starting values for the L2 regularization hyper-parameters. This should contain as many values as non-zero elements supplied to the argument `groupReg`, in addition to the number of smooth functions, if any. Default values are provided,
- `betaInit`: a vector of starting values for the regression weights. Default values are provided,
- `groupReg`: a list of length `L.formula` describing how the L2 regularization hyper-parameters in the *multiple parametric regression models*, i.e., non-smooth functions of inputs, should be grouped. Each element of `groupReg` is a vector referring to a formula in `L.formula` and contains the numbers of successive input variables in that formula whose regression weights share the same hyper-parameter. If the only term in a formula is an offset, then the corresponding element of `groupReg` should take the value `0`, so the corresponding regression weight will not be penalized. In the default `groupReg=NULL`, the regression weights of a smooth function of inputs share the same hyper-parameter, but different smooth functions are penalized by different hyper-parameters, and all the remaining non-smooth functions of inputs share the same hyper-parameter. For example, if we have `L.formula <- list(y ~ x1 + x2 + x3 + s(x1) + s(x2), ~ 1)`, then `groupReg=NULL` would correspond to one hyper-parameter penalizing the three regression weights of the triple `(x1, x2, x3)`, one hyper-parameter for the  regression weights of the smooth function `s(x1)`, one hyper-parameter for `s(x2)` and no hyper-parameter on the offset of the second output variable. However, if the regression weight of the input variable `x1` is constrained by an L2 penalty, and the regression weights of `x2` and `x3` share the same hyper-parameter, then the `groupReg` corresponding to that `L.formula` should be `groupReg <- list(c(1, 2), 0)`, where `1` corresponds to having one hyper-parameter on the regression weight of `x1`, `2` to having one hyper-parameter on the pair `(x2, x3)`, and `0` for the offset of the second output variable,
- `iterMax`: an integer for the number of maximal iterations in the optimization of the log-marginal likelihood and the penalized log-likelihood,
- `progressPen`: if `progressPen=TRUE`, information about the progress of the penalized log-likelihood maximization will be printed,
- `PenTol`: the tolerance in the maximization of the penalized log-likelihood, 
- `progressML`: if `progressML=TRUE`, information about the progress of the log-marginal likelihood maximization will be printed, 
- `MLTol`: the tolerance in the maximization of the log-marginal likelihood,
- `....`: additional arguments supplied to the function `gam()` in `mgcv` for setting up the input matrix and the smoothing matrices.

In the call to `mtgam`, the variable `fit` contains useful **outputs** for plots, predictions, etc..., as if `fit` resulted from the function `gam()` in `mgcv`. The only exception is the vector `sp`, which corresponds in `gam()` to the hyper-parameters for the smooth functions only, whereas in `mtgam`, `sp` contains the values of all the hyper-parameters including those described by the non-zero values in `groupReg`. Following the example given in the description of `groupReg` above, if we have `L.formula <- list(y ~ x1 + x2 + x3 + s(x1) + s(x2), ~ 1)` and `groupReg=NULL`, then `fit$sp` would be `(lamb1, lamb2, lamb3)`, where `lamb1` would be the hyper-parameter corresponding to the regression weights for `(x1, x2, x3)`, then `lamb2` would be associated to the regression weights of `s(x1)`, and `lamb3` to `s(x2)`. If `groupReg <- list(c(1, 2), 0)` then `fit$sp` would be `(lamb1, lamb2, lamb3, lamb4)`, where `lamb1` would be the hyper-parameter corresponding to the regression weight of `x1`, `lamb2` to the pair `(x2, x3)`, `lamb3` to `s(x1)` and `lamb4` to `s(x2)`. Further details can be found in [Section 2.2.3](#examples).

<a name="supportedDistrib"/></a>
### 2.2. Supported probability distributions and examples
The function `mtgam` supports any probability distribution whose log-likelihood is differentiable up to the third order and whose parametrization does not constrain the range values of the functional parameters. We describe the supported parametrizations in the following sections.

<a name="classical"/></a>
#### 2.2.1. Classical exponential family distributions
- Gaussian distribution: `fmName="gauss"` implements `N(mu, tau)`, where `mu` is the mean and `tau` is `2 log(sigma)` with `sigma` the standard deviation,
- Poisson distribution: `fmName="poisson"` implements `Poiss(mu)`, where `mu` is the log-rate,
- Exponential distribution: `fmName="expon"` implements `Expon(mu)`, where `mu` is the log-rate,
- Gamma distribution: `fmName="gamma"` implements `Gamma(mu, tau)`, where `mu` is the log-shape, `tau` is `-log(sigma)`, and `sigma` is the scale,
- Binomial distribution: `fmName="binom"` implements `Binom(mu)`, where `mu` is the logit, i.e., `log(p/(1-p))` with `p` the probability of success.

<a name="evd"/></a>
#### 2.2.2. Extreme value distribution families
- Generalized extreme value distribution: `fmName="gev"` implements `GEV(mu, tau, xi)`, where `mu` is the location, `tau` is the log-scale and `xi` is the shape,
- Generalized Pareto distribution: `fmName="gpd"` implements `GPD(mu, tau)`, where `tau` is the log-scale and `xi` is the shape,
- Point process approach in extreme value analysis: `fmName="pp"` implements `PP(mu, tau, xi)`, where `mu` is the location, `tau` is the log-scale and `xi` is the shape. The output variable `y` (say) in the argument `dat` of the function `mtgam` must be an nx(N+2)-matrix, where `n` is the number of blocks of data that are above the thresholds and `N` is the largest number of block exceedances. Each of the `n` rows of the data matrix `dat$y` corresponds to a block of data and must be filled accordingly. In particular, the first column of `dat$y` must contain the vector of the `n` block sizes, i.e., numbers of exceedances above the thresholds, the second column must be the vector of the `n` thresholds, and the remaining columns must be filled with the threshold exceedances and `NA` values when the size `n_i` of the `i`-th block contains fewer exceedances than `N`, i.e., when `n_i<N`. The `pp` model is still experimental,  
- r-Largest extreme value distribution: `fmName="rgev"` implements `rGEV(mu, tau, xi)`, where `mu` is the location, `tau` is the log-scale and `xi` is the shape. As the analogue of the point process approach, the output variable `y` (say) in the argument `dat` should be an nxr-matrix, where `n` is the number of blocks of data and `r` is the pre-specified number of largest extremal data per block. In particular, the values in the rows of `dat$y` must be sorted in ascending order.

Data from the families `gev`, `gpd` and `rgev` can be simulated using the function
```R
simExtrem(mu=NULL, sigma=NULL, xi=NULL, r=NULL, family="gev")
```
with **arguments**:
- `mu`: a vector of location parameters for the full dataset,
- `sigma`: a vector of scale parameters for the full dataset,
- `xi`: a vector of shape parameters for the full dataset,
- `r`: an integer for the number of r largest extremal data per block in the r-largest model,
- `family`: a character variable which takes either `"gev"`, `"gpd"` or `"rgev"`, 

and **output**: 
- if `family="gev"` or `family="gpd"`: a vector of generated data,
- if `family="rgev"`: a matrix of size `nxr`, where `n` is the length of `mu` and `r` is the number of r largest extremal data per block. The values in each of the rows are sorted in ascending order.


Return levels (quantiles) from the families `gev` and `gpd` can be computed by the function
```R
returnLevel(prob=NULL, mu=NULL, sigma=NULL, xi=NULL, family="gev")
```
with **arguments**:
- `prob`: a scalar for the probability for which the return level is computed,
- `mu`: a vector of location parameters for the full dataset,
- `sigma`: a vector of scale parameters for the full dataset,
- `xi`: a vector of shape parameters for the full dataset,
- `family`: a character variable which takes either `"gev"` or `"gpd"`, 

and **output**:
- a vector of return levels corresponding to the probability `prob` and the functional parameters `mu`, `sigma` and `xi`.

<a name="examples"/></a>
#### 2.2.3. Examples: 
The R file `examples.R` illustrates three key calls to `mtgam`:
  1. the usage of `groupReg` on the Gaussian model for example,
  2. the training of a multiple generalized additive models on the supported distributions,
  3. the definition of `dat` for the `pp` model (in pseudo-code).

<a name="newDistrib"/></a>
### 2.3. Extension to new distributions
New families of distributions can be implemented by the user and added to `multgam`, but for a numerically stable implementation, it is preferable to contact the author at yousra.elbachir@gmail.com who can do this for you.

<a name="comments"/></a>
## 3. General comments
- The package is under development and is currently being re-implemented with the object-oriented perspective in mind.
- The package has been tested with the following basis functions in the argument `L.formula` in `mtgam`: `bs="tp"` (thin plate regression splines), `bs="cr"` (cubic regression splines), `bs="cc"` (cyclic cubic regression splines).
- The point process approach in extreme value analysis, i.e. `fmName="pp"`, is still experimental.
- The convergence criteria are conservative. If the training does not converge, increase `MLTol` to `1e-06` or `1e5`. If this still does not converge, please report the error to the author following [Section 4](#bugs). 

<a name="bugs"/></a>
## 4. Bugs, help and suggestions
Bugs can be reported to yousra.elbachir@gmail.com by sending an email with:
- subject: multgam: bugs,
- content: a reproducible example and a simple description of the problem.

Further details on the usage of the package or suggestions for additional extensions can be requested to the author.

<a name="citation"/></a>
## 5. Citation
Acknowledge the use of `multgam` by referring to this web-page and citing the paper El-Bachir and Davison (2019) using (bibtex)
```
@article{JMLR:v20:18-659,
  author  = {El-Bachir, Y. and Davison, A. C.},
  title   = {Fast {A}utomatic {S}moothing for {G}eneralized {A}dditive {M}odels},
  journal = {Journal of Machine Learning Research},
  year    = {2019},
  volume  = {20},
  number  = {173},
  pages   = {1--27},
  url     = {http://jmlr.org/papers/v20/18-659.html}
}
```


## References
Yousra El-Bachir and Anthony C. Davison. Fast automatic smoothing for generalized additive models. *Journal of Machine Learning Research*, 20(173):1-27, 2019. Available at http://jmlr.org/beta/papers/v20/18-659.html.


