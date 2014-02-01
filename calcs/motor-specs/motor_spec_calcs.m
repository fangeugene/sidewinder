WHEEL_DIA = 2 * 2.54 / 100; % m
GEAR_REDUCTION = 1; % 1:GEAR_REDUCTION
ROBOT_MASS = 7; % kg
EFFICIENCY = 0.9;
SPEED_LOSS_CONSTANT = 0.9;
CoF = 0.7;
NUM_MODULES = 3;
% 
% MOTOR_CURVE = csvread('motor_specs/KIT-MTR-36-06-180.csv'); % [N*m, RPM]
% p1 = polyfit(MOTOR_CURVE(:, 2), MOTOR_CURVE(:, 1), 1);
% STALL_TORQUE = polyval(p1, 0); % N*m
% STALL_TORQUE = .5932; %N*m Pololu 37D 19:1
% STALL_TORQUE = .12; %N*m Pololu 25D 9.7:1
STALL_TORQUE = 1.4; %N*m Phidgets 3258E_0 10:1
% STALL_TORQUE = .13; %N*m Phidgets 3261E_0 3.7:1

% p2 = polyfit(MOTOR_CURVE(:, 1), MOTOR_CURVE(:, 2), 1);
% FREE_SPEED = polyval(p2, 0); % RPM
% FREE_SPEED = 500; %RPM Pololu 37D 19:1
% FREE_SPEED = 1010; %RPM Pololu 25D 9.7:1
FREE_SPEED = 365; %RPM Phidgets 3258E_0 10:1
% FREE_SPEED = 1080; %RPM Phidgets 3261E_0 3.7:1
TOP_LOAD_SPEED = SPEED_LOSS_CONSTANT * FREE_SPEED;

%% Calculating Top Speed
top_speed = (TOP_LOAD_SPEED / 60 / GEAR_REDUCTION) * pi * WHEEL_DIA
force_at_wheel = STALL_TORQUE * GEAR_REDUCTION / (WHEEL_DIA/2); % N

linear_accel = NUM_MODULES * force_at_wheel / ROBOT_MASS

%% Calculating Robot Rotation Speed
module_radius = 0.2; % m
rotational_top_speed = top_speed / module_radius;
MOI = 3 * ROBOT_MASS/3 * module_radius^2;
rotational_torque = 3*module_radius*force_at_wheel;
rotational_accel = rotational_torque/MOI;
%% Calculating Module Rotation Specs
b = 0.02;
h = 0.02;
i = b * h * (b^2 + h^2) / 12;
N = ROBOT_MASS * 9.81 / NUM_MODULES;
t = i * CoF * N;
