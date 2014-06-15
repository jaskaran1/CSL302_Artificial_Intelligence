function [Theta1,Theta2,trainingaccuracy,validationaccuracy]=mlptrain(X,y,X_val,y_val,K,alpha,times,numberofepochs)%assuming a single convergence rate
%times is used in the formula for calculating hidden_layer_size
%Called for training the mlp
%X doesn't contain the labels at the end
%X has 16 features
%Setup the layer parameters for the MLP
input_layer_size=size(X,2);%size of the feature vector without the bias
hidden_layer_size=times*input_layer_size;%the hidden layer size can be changed
%tripling the hidden layer size shot up accuracy by 5%.95% accuracy.
%max train accuracy achieved-99.18% at 15*inputlayer
num_labels=K;%the size of the output layer
%-------------------------------------------------------------
%Randomly initialize Theta1 and Theta2 weight matrices
epsilon_init=0.00012;
Theta1=rand(hidden_layer_size,input_layer_size+1)*2*epsilon_init-epsilon_init;
Theta2=rand(num_labels,hidden_layer_size+1)*2*epsilon_init-epsilon_init;
%-------------------------------------------------------------
%Stochastic gradient descent
%Training starts for 10 epochs.
%No vectorisation of code possible in stochastic gradient descent
%Since each example is coming one at a time to train the parameters
m=size(X,1);
format long;
ans1=0;
ans2=0;
trainingaccuracy=zeros(1,numberofepochs);
validationaccuracy=zeros(1,numberofepochs);
for epoch=1:numberofepochs
    randOrder=randperm(size(X,1))';
    shuffledX=X(randOrder,:);
    %shuffledX=X;%No shuffling
    shuffledlabels=y(randOrder,:);
    %alpha=320/m*exp(-epoch/numberofepochs);%The learning rate decreases with increase in number of epochs
%     if epoch<=10
%         alpha=110/m;%best accuracy at 120/m and 200 iterations:getting approx 90% accuracy
%     elseif epoch<=20
%         alpha=60/m;
%     elseif epoch<=40
%         alpha=40/m;
%     elseif epoch<=60
%         alpha=8/m;
%     elseif epoch<=100
%         alpha=1/m;


%     elseif epoch<=90 
%         alpha=1/(5*m);
%     else 
%         alpha=1/(10*m);

%    end    
    cost=0;
    iter=m;
    for i=1:iter %all the training examples 
        A1i=shuffledX(i,:)';%nx1 vector
        A1i=[1;A1i];%Add the bias term
        Z2i=Theta1*A1i;%hiddenlayersizex1
        A2i=sigmoid(Z2i);%activation value of hidden layer for the current example  
        %Zi is a vector of size hidden_layer_size X 1
        A2i=[1;A2i];%for bias.Now size becomes  (hidden_layer_size+1)X1
        %Yi=softmax(Theta2*Zi);%Kx1 vector for the training example
        Z3i=Theta2*A2i;
        A3i=sigmoid(Z3i);%Kx1 vector.Last layer activation value
        %A3i=softmax(Z3i);
        h=A3i;
        Yi=zeros(K,1);%Actual value of last layer is Yi
        Yi(shuffledlabels(i))=1;
        delta_3=A3i-Yi;%error in the output layer.Kx1
        Z2i=[1;Z2i];%introduce 1 in the 2nd layer for calculation of delta_2
        delta_2=Theta2'*delta_3.*sigmoidGradient(Z2i);
        delta_2=delta_2(2:end,:);%Hx1
        del2=delta_3*A2i';%the change in Theta2 matrix due to single training example
        del1=delta_2*A1i';%the change in Theta1 matrix due to single training example
        %the deltas for both the matrices have been calculated
        %Update the Thetas
        Theta1=Theta1-alpha*del1;
        Theta2=Theta2-alpha*del2;
        cost=cost+sum(-Yi.*log(h)-(1-Yi).*log(1-h));
    end
 J=(1/iter)*cost;
 fprintf('J(theta1,theta2) in epoch %d= %f\n',epoch,J);
     
%Check prediction accuracy in every epoch by calling the predict function
pred=predict(Theta1,Theta2,shuffledX);%shuffledX mx(n+1)
trainingaccuracy(1,epoch)=mean(double(pred == shuffledlabels)) * 100;
fprintf('Training Set Accuracy in epoch %d,hiddenlayersize=%d,learning rate=%f: %f\n',epoch,times,alpha,mean(double(pred == shuffledlabels)) * 100);
ans1=ans1+mean(double(pred == shuffledlabels)) * 100;
pred=predict(Theta1,Theta2,X_val);
validationaccuracy(1,epoch)=mean(double(pred ==y_val)) * 100;
fprintf('Validation Accuracy in epoch %d,hiddenlayersize=%d,learning rate=%f: %f\n',epoch,times,alpha,mean(double(pred==y_val))*100);
ans2=ans2+mean(double(pred==y_val))*100;
end
fprintf('Average training accuracy for learning rate=%f,hiddenlayersize=%d: %f\n',alpha,times,ans1/numberofepochs);
fprintf('Average validation accuracy for learning rate=%f,hiddenlayersize=%d: %f\n',alpha,times,ans2/numberofepochs);
% figure;
% plot(1:numberofepochs,trainingaccuracy,'r+:',1:numberofepochs,validationaccuracy,'g+:');
% title('Accuracy Vs Epochs')
% xlabel('Epochs');
% ylabel('Accuracy');
% legend('Training','Validation','Location','SouthEast');
% axis equal;
end