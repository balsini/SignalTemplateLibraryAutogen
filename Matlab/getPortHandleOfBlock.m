function [ portHandle ] = getPortHandleOfBlock( block , signal)
%GETLINESOFBLOCK Summary of this function goes here
%   Detailed explanation goes here

portHandle = {};

ports = get_param(block,'PortHandles');
line = get_param(ports(1).Outport,'Line');

if (iscell(line))
    if (size(line, 1) == 1)
        signalName = get_param(line(1), 'Name');
        if (strcmp(signalName, signal))
            portHandle = get_param(line(1), 'Srcporthandle');% line{j};
            return
        end
    end
    if (size(line, 1) > 1)
        for k=1:size(line, 1)
            signalName = get_param(line{k}, 'Name');
            if (strcmp(signalName, signal))
                portHandle = get_param(line{k}, 'Srcporthandle');% portHandle = line{k};
                return
            end
        end
    end
else
    if line ~= -1
        signalName = get_param(line, 'Name');
        if (strcmp(signalName, signal))
            portHandle = get_param(line, 'Srcporthandle');% line{j};
            return
        end
    end
end
end

