## Copyright (C) 2023 Stefano D'Angelo
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in
## all copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
## THE SOFTWARE.

## -*- texinfo -*-
## @deftypefn {Function File} {@var{y} =} triode_stage_quadric (@var{fs}, @var{x}, @var{kp}, @var{kp2}, @var{kpg}, @var{E}, @var{Rp}, @var{Rk}, @var{Ck}, @var{Rg})
##
## Simulate the common-cathode triode stage circuit described in
## @quotation
## R. Giampiccolo, S. D'Angelo, A. Bernardini, and A. Sarti, ``A Quadric Surface
## Model of Vacuum Tubes for Virtual Analog Applications,''
## @emph{submitted paper}.
## @end quotation
##
## It produces the output vector @var{y} from the audio input vector @var{x}.
## @var{fs} is the sample rate (in Hz).
##
## @var{kp}, @var{kp2}, and @var{kpg} are scalars representing the tube
## parameters.
##
## @var{E}, @var{Rp}, @var{Rk}, @var{Ck}, and @var{Rg} are scalars representing
## circuit component values.
##
## @end deftypefn

## Author: Stefano D'Angelo <zanga.mail@gmail.com>
## Maintainer: Stefano D'Angelo <zanga.mail@gmail.com>
## Version: 1.0.0
## Keywords: vacuum tube triode guitar amplifier

function y = triode_stage_quadric(fs, x, kp, kp2, kpg, E, Rp, Rk, Ck, Rg)

  #### Input arguments

  # Checks

  if (nargin != 10)
    print_usage();
  elseif (!isscalar(fs) || !isnumeric(fs) || !isreal(fs) || fs <= 0)
    error("triode_stage_quadric: FS must be a positive real");
  elseif (isscalar(x) || !isvector(x) || !isnumeric(x) || !isreal(x))
    error("triode_stage_quadric: X must be a vector of real values");
  elseif (!isscalar(kp) || !isnumeric(kp) || !isreal(kp))
    error("triode_stage_quadric: KP must be a real scalar");
  elseif (!isscalar(kp2) || !isnumeric(kp2) || !isreal(kp2))
    error("triode_stage_quadric: KP2 must be a real scalar");
  elseif (!isscalar(kpg) || !isnumeric(kpg) || !isreal(kpg))
    error("triode_stage_quadric: KPG must be a real scalar");
  elseif (!isscalar(E) || !isnumeric(E) || !isreal(E))
    error("triode_stage_quadric: E must be a real scalar");
  elseif (!isscalar(Rp) || !isnumeric(Rp) || !isreal(Rp))
    error("triode_stage_quadric: RP must be a real scalar");
  elseif (!isscalar(Rk) || !isnumeric(Rk) || !isreal(Rk))
    error("triode_stage_quadric: RK must be a real scalar");
  elseif (!isscalar(Ck) || !isnumeric(Ck) || !isreal(Ck))
    error("triode_stage_quadric: CK must be a real scalar");
  elseif (!isscalar(Rg) || !isnumeric(Rg) || !isreal(Rg))
    error("triode_stage_quadric: RG must be a real scalar");
  endif

  # Enforce bounds

  E  = limit(E,  1e-6,  inf, "E");
  Rp = limit(Rp, 1e-6,  inf, "Rp");
  Rk = limit(Rk, 1e-6,  inf, "Rk");
  Ck = limit(Ck, 1e-18, inf, "Ck");

  #### Constants

  # Circuit components

  Ri = 1e6;
  Ci = 100e-9;
  Ro = 1e6;
  Co = 10e-9;

  # Initial voltages
  
  k1 = kpg / (2 * kp2) + Rp / Rk + 1;
  k2 = k1 * (kp / kp2 + 2 * E) * kp2;
  k3 = Rk * k2 + 1;
  Vk0 = (k3 - sign(k1) * sqrt(2 * k3 - 1)) / (2 * Rk * k1 * k1 * kp2);
  Vp0 = E - Rp / Rk * Vk0;

  # WDF element values

  wVi_R  = 1e-6;
  wCi_R  = 1 / (2 * fs * Ci);
  wCk_R  = 1 / (2 * fs * Ck);
  wCo_R  = 1 / (2 * fs * Co);
  wsi_kl = wCi_R / (wCi_R + wVi_R);
  wsi_R  = wCi_R + wVi_R;
  wpg_kt = wsi_R / (wsi_R + Ri);
  wpg_R  = (wsi_R * Ri) / (wsi_R + Ri);
  wsg_kl = Rg / (Rg + wpg_R);
  wsg_R  = Rg + wpg_R;
  wpk_kt = wCk_R / (Rk + wCk_R);
  wpk_R  = (Rk * wCk_R) / (Rk + wCk_R);
  wsp_kl = wCo_R / (wCo_R + Ro);
  wsp_R  = wCo_R + Ro;
  wpp_kt = wsp_R / (wsp_R + Rp);
  wpp_R  = (wsp_R * Rp) / (wsp_R + Rp);

  # Filter coefficients

  kTxCi  = 1 - wpg_kt;
  kTCk   = 1 - wpk_kt;
  kTCo   = 1 - wpp_kt;
  kT0    = wpp_kt * E;
  kyT    =  0.5 * (1 - wsp_kl);
  kyCo   = -0.5 * (1 - wsp_kl) * (1 + wpp_kt);
  ky0    =  0.5 * (1 - wsp_kl) * wpp_kt * E;
  kCiT   = wsi_kl * (1 - wsg_kl);
  kCixCi = wsi_kl * ((1 - wpg_kt) * (wsg_kl + 1) - 2);
  kCoCo  = 1 - wsp_kl * (1 + wpp_kt);
  kCo0   = wsp_kl * wpp_kt * E;

  #### Filter

  y     = zeros(1, length(x));
  wCi_s = 0;
  wCk_s = Vk0;
  wCo_s = Vp0;

  for i = 1:length(x)

    xCi   = x(i) + wCi_s;
    wT_ag = kTxCi * xCi;
    wT_ak = kTCk * wCk_s;
    wT_ap = kTCo * wCo_s + kT0;
    [wT_bg, wT_bk, wT_bp] = triode(wT_ag, wT_ak, wT_ap, wsg_R, wpk_R, wpp_R,
                                   kp, kp2, kpg);
    y(i)  = kyT * wT_bp + kyCo * wCo_s + ky0;
    wCi_s = kCiT * wT_bg + kCixCi * xCi + wCi_s;
    wCk_s = wT_bk - wpk_kt * wCk_s;
    wCo_s = wsp_kl * wT_bp + kCoCo * wCo_s + kCo0;

  endfor

endfunction

function [bg, bk, bp] = triode(ag, ak, ap, R0g, R0k, R0p, kp, kp2, kpg)
  # These could be precomputed
  bk_bp = R0k / R0p;
  k_eta = 1 / (bk_bp * (0.5 * kpg + kp2) + kp2); # = 1 / (2 * kp2 * gamma)
  k_delta = kp2 * k_eta * k_eta / (R0p + R0p);
  k_bp_s = k_eta * sqrt((kp2 + kp2) / R0p);
  bp_k = 1 / (R0p + R0k);
  bp_ap_0 = bp_k * (R0k - R0p);
  bp_ak_0 = bp_k * (R0p + R0p);
  
  v1 = 0.5 * ap;
  v2 = ak + v1 * bk_bp;
  alpha = kpg * (ag - v2) + kp;
  beta = kp2 * (v1 - v2);
  eta = k_eta * (beta + beta + alpha);
  v3 = eta + k_delta;
  delta = ap + v3;
  
  if (delta >= 0)
    bp = k_bp_s * sqrt(delta) - v3 - k_delta;
    d = bk_bp * (ap - bp);
    bk = ak + d;
    Vpk2 = ap + bp - ak - bk;
    
    if (kpg * (ag - ak - 0.5 * d) + kp2 * Vpk2 + kp < 0)
      bp = ap;
      bk = ak;
      Vpk = ap - ak;
    else
      Vpk = 0.5 * Vpk2;
    endif
  else
    bp = ap;
    bk = ak;
    Vpk = ap - ak;
  endif
  
  if (Vpk < 0)
    bp = bp_ap_0 * ap + bp_ak_0 * ak;
  endif
  
  bg = ag;
endfunction

function y = limit(x, low, up, name)
  y = x;
  if (min(y) < low || max(y) > up)
    warning(["triode_stage: limiting " name " to [" num2str(low) ", " ...
             num2str(up) "]"]);
    y(y < low) = low;
    y(y > up)  = up;
  endif
endfunction
