function [ ROOT ] = main( SYSTEM , NAME , ROOT_POSITION )
%clear all
%SYSTEM = 'provaSimulink';
%NAME = 'STL_TEST';

simulink;
load_system('simulink');
load_system('STLlib');
open_system(SYSTEM);

ROOT = [SYSTEM '/' NAME];
add_block('simulink/Ports & Subsystems/Subsystem', ROOT);
delete_line(ROOT,'In1/1','Out1/1');
delete_block([ROOT '/Out1']);
delete_block([ROOT '/In1']);
set_param(ROOT,'position', ROOT_POSITION);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
AUTOGEN_testBlock;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

end