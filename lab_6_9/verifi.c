#include<iostream>
using namespace std;

int main(){
    // Driver program
    int n,x;
    cin >> n >> x;  // Taking inputs
    if(n<=1000 && x<=500){
    int A[n],sum = 0;
    for(int i = 0;i<n;i++){  // Taking inputs into the array
        cin >> A[i];
        sum += A[i];
    }
    int divm[x];
    bool dp[sum + 1][n+1];  // Initiation of table named dp

 // Base conditions
    for(int i = 0;i< sum + 1;i++){
        dp[i][0] = 0;
    }
    for(int j = 0; j < n + 1;j++){
        dp[0][j] = 1;
    }

 // Table values calculation with Recursion
for(int i = 1;i<sum+1;i++){
    for(int j = 1;j< n+1;j++){
        if(A[j] > i){
            dp[i][j] = 0;
        }else{
            dp[i][j] = dp[i-A[j]][j-1] || dp[j-1][i];
        }
    }
}

 // Storing 1 if the subpart satisfies the given condition
 // given condition otherwise storing 0
for(int i = 1;i<sum+1;i++){
    for(int j = 1;j<n+1;j++){
        if (dp[i][j] == 1){
         divm[x%i] = 1;
        }
}
}

// Print output
if(divm[0] == 1){
    cout << "Yes\n";
}else{
    cout << "No\n";
}
return 0;
    }
}


// code written by Sravanth 2101AI29
