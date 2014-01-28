function [mag, ang] = independent_module_control(module_r, module_theta, translation_mag, translation_ang, rotation_vel)
    % computes the magnitude and direction (relative to the world frame) a
    % module needs to be at in order to achieve the desired velocity and
    % rotational velocity.

    rotation_component_mag = module_r * rotation_vel;
    rotation_component_ang = module_theta + pi/2;

    translation_component_mag = translation_mag;
    translation_component_ang = translation_ang;

    [mag, ang] = add_polar_vectors(translation_component_mag, translation_component_ang, rotation_component_mag, rotation_component_ang);

end
