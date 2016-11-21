function [ portHandle ] = getSourcePortHandleOfSignal( SYSTEM , blocks , signal )
%GETSOURCEPORTHANDLEOFSIGNAL Summary of this function goes here
%   Detailed explanation goes here

for i=1:size(blocks, 1)
    block = blocks{i};
    if (strcmp(SYSTEM, block) == 0)
        portHandle = getPortHandleOfBlock(block , signal);
        
        if (size(portHandle) > 0)
            %block
            %portHandle
            return
        end
        
        %{
        %get_param(getSimulinkBlockHandle(block,true),'Name')
        b = get_param(block,'PortConnectivity');
        %cellfun('length',b)
        DST = b{1,1}.DstBlock;
        if (size(DST) > 0)
            block
            %DST
            outPorts = get_param(DST, 'Name')
            outPorts = get_param(DST, 'Label')
        end
        
        n = numel(b) % number of blocks connected to block1;
        for k = 1:n
            b(k,1)
            s = get(b(k).SrcBlock);
            f = 'Source';
            if isempty(s)
                s=get(b(k).DstBlock);
                f='Destinataion';
            end
            out{k,1}=f;
            out{k,2}=s.BlockType;
            out{k,3}=s.Name
        end
        disp(out)  % the first column indicate if a block is a source or a destination, the second indicate the type, and the third is the name.
        %}
    end
end

end

