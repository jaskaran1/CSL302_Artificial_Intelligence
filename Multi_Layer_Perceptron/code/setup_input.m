function [X,Xtst,K]=setup_input()
%X is feature matrix
%K is the number of classes for classification
fin=fopen('p3mlptrainingdata.txt');
X=[];
while ~feof(fin)
currline=fgetl(fin);
C=regexp(currline,',','split');
C=cellfun(@str2num,C);
X=[X;C];
end
fclose(fin);
Y=X(:,end);%The last column of the data matrix are labels.
%not extracting this since we'll be randomizing the matrix.
K=length(unique(Y));%no of classes
%Take input for the test data
fin=fopen('p3mlptstdata.txt');
Xtst=[];
while ~feof(fin)
currline=fgetl(fin);
C=regexp(currline,',','split');
C=cellfun(@str2num,C);
Xtst=[Xtst;C];
end
fclose(fin);
end