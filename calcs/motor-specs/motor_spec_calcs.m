WHEEL_DIA = 4 * 2.54 / 100; % m
GEAR_REDUCTION = 1; % 1:GEAR_REDUCTION
ROBOT_MASS = 7; % kg
EFFICIENCY = 0.9;
SPEED_LOSS_CONSTANT = 0.9;
CoF = 0.7;
NUM_MODULES = 3;

MOTOR_CURVE = csvread('motor_specs/KIT-MTR-36-06-180.csv'); % [N*m, RPM]
p1 = polyfit(MOTOR_CURVE(:, 2), MOTOR_CURVE(:, 1), 1);
STALL_TORQUE = polyval(p1, 0); % N*m
p2 = polyfit(MOTOR_CURVE(:, 1), MOTOR_CURVE(:, 2), 1);
FREE_SPEED = polyval(p2, 0); % RPM
TOP_LOAD_SPEED = SPEED_LOSS_CONSTANT * FREE_SPEED;

%% Calculating Top Speed
top_speed = (TOP_LOAD_SPEED / 60 / GEAR_REDUCTION) * pi * WHEEL_DIA;
force_at_wheel = STALL_TORQUE * GEAR_REDUCTION / (WHEEL_DIA/2); % N

a = NUM_MODULES * force_at_wheel / ROBOT_MASS;

%% Calculating Robot Rotation Speed
module_radius = 0.5; % m
w = top_speed / module_radius;

%% Calculating Module Rotation Specs
b = 0.02;
h = 0.02;
i = b * h * (b^2 + h^2) / 12;
N = ROBOT_MASS * 9.81 / NUM_MODULES;
t = i * CoF * N;
