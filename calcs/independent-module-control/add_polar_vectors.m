function [mag, ang] = add_polar_vectors(mag1, ang1, mag2, ang2)
    % adds two vectors in polar form and returns them in polar form
    [x1, y1] = pol2cart(ang1, mag1);
    [x2, y2] = pol2cart(ang2, mag2);
    [ang, mag] = cart2pol(x1 + x2, y1 + y2);
end
