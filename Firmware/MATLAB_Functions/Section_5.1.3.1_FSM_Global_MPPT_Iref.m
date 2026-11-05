function Iref = mppt(Vpv, Ipv)
persistent V_target Pmax Vbest mode timer;

Ts = 1e-6;

% Voltage sweep parameters - sweep Vpv from high to low
V_oc_est = 280; % Estimated open circuit voltage
V_scan_min = 240; % Minimum scan voltage
V_ramp_rate = 150; % V/s sweep rate

if isempty(V_target)
    V_target = V_oc_est;
    Pmax = 0;
    Vbest = 240; % Start with nominal MPP voltage
    mode = 0;
    timer = 0;
end

Ppv = Vpv * Ipv;

switch mode
    case 0 % RESET - go to high voltage (low current)
        V_target = V_oc_est;
        Pmax = 0;
        if Vpv > 270 % Near Voc
            mode = 1;
        end

    case 1 % SCAN - sweep voltage DOWN to find global Pmax
        % Record power at each voltage
        if Ppv > Pmax
            Pmax = Ppv;
            Vbest = V_target;
        end

        V_target = V_target - (V_ramp_rate * Ts);

        if V_target <= V_scan_min
            mode = 2;
        end

    case 2 % RETURN to Vbest
        if V_target < Vbest
            V_target = V_target + (300 * Ts);
        else
            V_target = Vbest;
            mode = 3;
            timer = 0;
        end

    case 3 % HOLD
        timer = timer + Ts;
        if timer >= 5
            mode = 0;
        end
end

% INNER LOOP: Current controller to regulate Vpv to V_target
% Simple P-control: if Vpv > V_target, increase current (load more)
% if Vpv < V_target, decrease current (load less)
persistent err_int;
if isempty(err_int), err_int = 0; end

err = Vpv - V_target; % Positive = need more load
err_int = err_int + err * Ts;
err_int = max(min(err_int, 50), -50); % Anti-windup

Kp = 1; % Proportional gain
Ki = 200; % Integral gain

% Base current + correction
I_calc = 20 + Kp * err + Ki * err_int;

% Limit output
Iref = max(min(I_calc, 35), 0);
end
