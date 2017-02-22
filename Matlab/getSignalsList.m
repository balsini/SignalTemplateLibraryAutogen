function [ ROOT ] = mainPortList( SYSTEM , NAME )

%clear all
%SYSTEM = 'provaSimulink';

fileName = 'AUTOGEN_portList.txt';

simulink;
load_system('simulink');
open_system(SYSTEM);

fileID = fopen(fileName,'w');

% All the lines
L = find_system(SYSTEM,'FindAll','on','type','line');

% Signal names
n = containers.Map;

for i=1:size(L,1)
    conn = get_param(L(i), 'Connected');
    name = get_param(L(i), 'Name');
        
    if (size(name, 2) > 0 && strcmp(conn, 'on'))
        n(get_param(L(i), 'Name')) = 1;
        
        %get_param(L(i),'ObjectParameters')
        %get_param(L(i),'PortConnectivity')
    end
end

k = keys(n);

for i=1:size(k,2)
    fprintf(fileID, '%s\n', strjoin(k(i)));
end
end
