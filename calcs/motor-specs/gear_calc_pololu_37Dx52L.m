clc; clear; close all;

%% Possibilities
Gears = [52 28;48 32;44 36;40 40];
Gearing = Gears(:,2)./Gears(:,1);
Wheels = [1.875;2];

for i = 1:length(Wheels)
    for j = 1:length(Gearing)
        [MaxSpeedL(i,j),MaxAccelL(i,j),Z2Speed(i,j),MaxSpeedR(i,j),MaxAccelR(i,j)] ...
    = motor_calc_pololu_37Dx52L(Wheels(i),Gearing(j));
    end
end

figure
plot(Gearing,MaxSpeedL(1,:),'-x',Gearing,MaxSpeedL(2,:),'--o')
xlabel('Gear Reduction')
ylabel('Speed (m/s)')
legend(num2str(Wheels))

figure
plot(Gearing,MaxAccelL(1,:),'-x',Gearing,MaxAccelL(2,:),'--o')
xlabel('Gear Reduction')
ylabel('Accel (m/s^2)')
legend(num2str(Wheels),'Location','NorthWest')

figure
plot(Gearing,Z2Speed(1,:),'-x',Gearing,Z2Speed(2,:),'--o')
xlabel('Gear Reduction')
ylabel('Time (s)')
title('Zero to 95% Speed')
legend(num2str(Wheels))