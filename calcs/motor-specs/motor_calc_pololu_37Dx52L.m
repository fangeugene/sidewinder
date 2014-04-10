function [max_speed_loaded,max_linear_accel,zero2speed,max_speed_rot,rotational_accel] ...
    = motor_calc_pololu_37Dx52L(WHEEL_DIA,GEAR_REDUCTION)
% General params
NUM_MODULES = 3;
ROBOT_MASS = 5; % kg
CoF = 0.5;
friction_cutoff = CoF * ROBOT_MASS * 9.8; % N

% Module params
WHEEL_DIA = WHEEL_DIA * 2.54 / 100; % m
% GEAR_REDUCTION = 0.75; % 1:GEAR_REDUCTION
SPEED_LOSS_CONSTANT = 0.8;

% Motor params
STALL_TORQUE = 0.59; %N*m pololu_37Dx52L 19:1
FREE_RPM = 500; %RPM pololu_37Dx52L 19:1
p = polyfit([0; FREE_RPM], [STALL_TORQUE; 0], 1);

% Sim params
SIM_SECONDS = 1;
SIM_TIMESTEP = 0.0001; % s
sim_num_timesteps = SIM_SECONDS / SIM_TIMESTEP;

%% Shared calcs
max_rpm_loaded = SPEED_LOSS_CONSTANT * FREE_RPM;
max_speed_loaded = (max_rpm_loaded / 60 / GEAR_REDUCTION) * pi * WHEEL_DIA;
stall_force_at_wheel = STALL_TORQUE * GEAR_REDUCTION / (WHEEL_DIA/2); % N

%% Calculating linear specs
max_linear_accel = NUM_MODULES * stall_force_at_wheel / ROBOT_MASS;

loaded_torque = polyval(p, max_rpm_loaded);
loaded_force = loaded_torque * GEAR_REDUCTION / (WHEEL_DIA/2);

% back calculating the resistance factor
resistance_factor = loaded_force / max_speed_loaded; % N / m/s
resistance_factor_t = resistance_factor * (WHEEL_DIA/2) / GEAR_REDUCTION; % N*m / m/s

cur_speed = 0;
cur_rpm = cur_speed / pi / WHEEL_DIA * GEAR_REDUCTION * 60;
cur_time = 0;
speeds = zeros(sim_num_timesteps, 1);
times = zeros(sim_num_timesteps, 1);
zero2speed = 0;
for i=1:sim_num_timesteps
    torque = polyval(p, cur_rpm);
    torque = torque - cur_speed * resistance_factor_t;
    force = NUM_MODULES * torque * GEAR_REDUCTION / (WHEEL_DIA/2);
    
    % limit force based on friction
    if force > friction_cutoff
        force = friction_cutoff;
    end
    
    % calculate change in speed for this timestep
    accel = force / ROBOT_MASS;
    d_speed = accel * SIM_TIMESTEP;
    
    cur_speed = cur_speed + d_speed;
    cur_rpm = cur_rpm + d_speed / WHEEL_DIA / pi * GEAR_REDUCTION * 60;
    
    % save zero to 95% speed time
    if zero2speed == 0 && cur_speed >= 0.95 * max_speed_loaded
        zero2speed = cur_time;
    end
    
    speeds(i) = cur_speed;
    times(i) = cur_time;
    
    cur_time = cur_time + SIM_TIMESTEP;
end

% figure(1);
% hold on;
% plot(times, speeds);
% plot(times, ones(size(times)) * 0.95 * max_speed_loaded, 'k--');
% xlabel('Time (s)');
% ylabel('Speed (m/s)');
% hold off;

% disp(['Max linear speed: ', num2str(max_speed_loaded), ' m/s']);
% disp(['Max linear acceleration: ', num2str(max_linear_accel), ' m/s^2']);
% disp(['Zero to 95% max speed in: ', num2str(zero2speed), ' s']);

%% Calculating rotation specs
module_radius = 0.2; % m
max_speed_rot = max_speed_loaded / module_radius;
MOI = 3 * ROBOT_MASS/3 * module_radius^2;
rotational_torque = 3*module_radius*stall_force_at_wheel;
rotational_accel = rotational_torque/MOI;

% disp(['Max rotational speed: ', num2str(max_speed_rot), ' rad/s']);
% disp(['Max rotational acceleration: ', num2str(rotational_accel), ' rad/s^2']);

end

