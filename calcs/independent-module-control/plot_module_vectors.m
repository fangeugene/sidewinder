clc, clear

size = 10;
xs = zeros(2*size + 1, 2*size + 1);
ys = zeros(2*size + 1, 2*size + 1);
vxs = zeros(2*size + 1, 2*size + 1);
vys = zeros(2*size + 1, 2*size + 1);

for xi=-size:size
    x = xi / size;
    for yi=-size:size
        y = yi / size;

        xs(xi+size + 1, yi+size + 1) = x;
        ys(xi+size + 1, yi+size + 1) = y;        

        [module_theta, module_r] = cart2pol(x, y);
        [mag, ang] = independent_module_control(module_r, module_theta, 1, pi/4, pi/4);
        [vx, vy] = pol2cart(ang, mag);

        vxs(xi+size + 1, yi+size + 1) = vx;
        vys(xi+size + 1, yi+size + 1) = vy;

    end
end

figure(1);
axis('equal');
quiver(xs, ys, vxs, vys);
