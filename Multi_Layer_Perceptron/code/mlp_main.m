%Setup the input for the MLP
[X,Xtst,K]=setup_input();%X contains the training labels in the last column
%Xtst contains the test data set
%Stratified Cross Validation
% List out the category values in use.
categories =(1:K)';

% Get the number of vectors belonging to each category.
%X(:,end) are the labels are the last column in X
vecsPerCat = getVecsPerCat(X(:,end),categories);

% Compute the fold sizes for each category.
numfolds=2;%Number of folds
foldSizes = computeFoldSizes(vecsPerCat, numfolds);
%foldSizes is a matrix #categories X #folds
% Randomize the vectors in X, then organize them by category.
y=X(:,end);
X=X(:,1:end-1);%stripped of the last column of labels
m=size(X,1);
m=m/numfolds;%number of training examples in each fold approximately
%m is used in the learning parameter
[X_sorted, y_sorted] = randSortAndGroup(X,y,categories);
bestalpha=-1;
besttimes=-1;
bestaccuracy=-1;
besttrain=[];
bestvalid=[];
numberofepochs=100;
resultmatrix=[];
for times=5:5:15
for alp=15:5:40  
        alpha=alp/m;
% For each round of cross-validation…
train=zeros(1,numberofepochs);
validation=zeros(1,numberofepochs);
res=0;
for roundNumber = 1 : numfolds 
fprintf('Cross validation round %d\n',roundNumber);
% Select the vectors to use for training and cross validation.
[X_train, y_train, X_val, y_val] = getFoldVectors(X_sorted, y_sorted, categories, vecsPerCat, foldSizes, roundNumber);

% Train the classifier on the training set, X_train y_train
% …………………
fprintf('Training....\n');
[Theta1,Theta2,trainingaccuracy,validationaccuracy]=mlptrain(X_train,y_train,X_val,y_val,K,alpha,times,numberofepochs);%Trains
% Measure the classification accuracy on the validation set.
% …………………
train=train+trainingaccuracy;
validation=validation+validationaccuracy;
pred=predict(Theta1,Theta2,X_val);
res=res+mean(double(pred == y_val)* 100);
end
train=train/numfolds;
validation=validation/numfolds;
resultmatrix=[resultmatrix;times,alpha,mean(train),mean(validation)];
if(bestaccuracy<res/numfolds)
    bestaccuracy=res/numfolds;
    bestalpha=alpha;
    besttimes=times;
    besttrain=train;
    bestvalid=validation;
end
% plot(1:numberofepochs,train,'r+:');
% hold on;
% plot(1:numberofepochs,validation,'g+:');
% %h=[h plot(1:numberofepochs,train,'r+:') plot(1:numberofepochs,validation,'g+:')];
% %legendset=[legendset;sprintf('train-times=%d,alpha=%f',times,alpha) sprintf('valid-times=%d,alpha=%f',times,alpha)];
% hold on;
end
end
%legend(h,legendset,'Location','SouthEast');
% title('Accuracy Vs Epochs')
% xlabel('Epochs');
% ylabel('Accuracy');
% axis equal;
% hold off;
fprintf('Best Cross Validation Accuracy: %f on alpha=%f times=%d',bestaccuracy,bestalpha,besttimes);
%figure;
plot(1:numberofepochs,besttrain,'r+:',1:numberofepochs,bestvalid,'g+:');
title('Accuracy Vs Epochs')
xlabel('Epochs');
ylabel('Accuracy');
legend('Training','Validation','Location','SouthEast');
axis equal;
%Best Cross Validation Accuracy: 97.363041 on alpha=0.011655 times=15
%resultmatrix%Uncomment to get the result matrix
%Labels for the test data are written to the file testlabels.txt
[Theta1,Theta2]=mlptesttrain(X,y,K,bestalpha,besttimes,numberofepochs);
labels=predict(Theta1,Theta2,Xtst);
fout=fopen('test_labels.txt','wt');
for i=1:length(labels)
fprintf(fout,'%d\n',labels(i));
end
fclose(fout);
