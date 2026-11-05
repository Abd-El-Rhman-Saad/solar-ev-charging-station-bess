function [g1, g2] = mppt_controller_pwm(Vpv, Ipv, IL1)
persistent V_target Pmax Vbest mode timer;
persistent err_int_v err_int_i t;

Ts = 1e-6;
fsw = 10e3;
Tsw = 1/fsw;

if isempty(V_target)
    V_target = Vpv;
    Pmax = 0;
    Vbest = Vpv;
    mode = 0;
    timer = 0;
    err_int_v = 0;
    err_int_i = 0;
    t = 0;
end

% ===== Internal Calculation of IL2 =====
% Based on the nodal equation: Ipv = IL1 + IL2
IL2 = Ipv - IL1;

% ===== MPPT Logic (Scanning) =====
Ppv = Vpv * Ipv;
V_ramp_rate = 200;

switch mode
    case 0 % Initialization
        V_target = Vpv;
        Pmax = 0;
        mode = 1;

    case 1 % Scanning Downward
        if Ppv > Pmax
            Pmax = Ppv;
            Vbest = Vpv;
        end
        V_target = V_target - (V_ramp_rate * Ts);
        % Stop scan at 50% of starting voltage
        if V_target <= 0.5 * Vbest
            mode = 2;
        end

    case 2 % Return to Vbest
        V_target = Vbest;
        mode = 3;
        timer = 0;

    case 3 % Hold and Dwell
        timer = timer + Ts;
        if timer >= 5
            mode = 0; % Restart scan
        end
end

% ===== Outer Loop: Voltage Control (Vpv -> Iref) =====
err_v = Vpv - V_target;

Kp_v = 1.5;
Ki_v = 80;

err_int_v = err_int_v + err_v * Ts;
err_int_v = max(min(err_int_v, 0.5), -0.5);

Iref_total = Kp_v * err_v + Ki_v * err_int_v;
Iref_total = max(min(Iref_total, 20), 0);

% ===== Inner Loop: Current Control (Iref -> Duty) =====
Iref_phase = 0.5 * Iref_total;

err_i1 = Iref_phase - IL1;
err_i2 = Iref_phase - IL2;

Kp_i = 5;
Ki_i = 100;

err_int_i = err_int_i + (err_i1 + err_i2) * Ts;
err_int_i = max(min(err_int_i, 10), -10);

% Duty cycle calculation
duty1 = Kp_i * err_i1 + Ki_i * err_int_i;
duty2 = Kp_i * err_i2 + Ki_i * err_int_i;

% Safety Clamps
duty1 = max(min(duty1, 0.9), 0.1);
duty2 = max(min(duty2, 0.9), 0.1);

% ===== PWM Generation (180 deg Phase Shift) =====
t = t + Ts;
if t >= Tsw, t = 0; end

carrier1 = t / Tsw;
carrier2 = mod(t + Tsw/2, Tsw) / Tsw;

% ===== Gate signals =====
g1 = (duty1 > carrier1);
g2 = (duty2 > carrier2);
end
