Suppose that we have a nonlinear system defined by:

$$
\begin{align}
&\text{process model:} \qquad\qquad  x_{k+1} = f(x_k, u_k) + w_k \qquad  \\
&\text{measurement model:}\qquad z_k = h(x_k) + v_k
\end{align} \tag{1}
$$

where:

- $x_k\in \mathbb{R}^m:$ state vector
- $z_k \in \mathbb{R}^n:$ measurement vector
- $u_k\in\mathbb{R}^l:$ control input vector
- $f: \mathbb{R}^m \times \mathbb{R}^l \rightarrow \mathbb{R}^m, f\in C^1:$ process model
- $h:\mathbb{R}^m \rightarrow \mathbb{R}^n, h \in C^1:$ measurement model
- $w_k\sim\mathcal{N}(0, Q):$ process noise
- $v_k\sim\mathcal{N}(0, R):$ measurement noise
- $Q\in\mathbb{R}^{m\times m}:$ process noise covariance
- $R\in\mathbb{R}^{n\times n}:$ measurement noise covariance
- $P_k\in\mathbb{R}^{m\times m}:$ state estimation error covariance at step $k$.

At each step, prediction takes the previous posterior estimate $\hat{x}_{k-1|k-1}$ to the current prior estimate $\hat{x}_{k|k-1}$. The measurement update then produces the current posterior estimate $\hat{x}_{k|k}$.

Important equations are boxed.

## Prediction Step

### 1. Predict prior state forward, $\hat{x}_{k|k-1}$

$$\boxed{\hat{x}_{k|k-1}:=f(\hat{x}_{k-1|k-1},u_{k-1})} \tag{2}$$

### 2. Predict prior estimation-error covariance, $P_{k|k-1}$

Define the prior prediction error to be:

$$
\begin{align}
\delta_{k|k-1}&=x_k -\hat{x}_{k|k-1} \\
&= f(x_{k-1}, u_{k-1}) + w_{k-1} - f(\hat{x}_{k-1|k-1}, u_{k-1})\qquad\text{(using eq'n 1 and 2)} \tag{A}
\end{align}
$$

where $x_k$ is the true (but unknown) state of the system.
Now, linearize $f$ about $\hat{x}_{k-1|k-1}:$

$$
\begin{align}
\Rightarrow f(x_{k-1}, u_{k-1}) &\approx f(\hat{x}_{k-1|k-1}, u_{k-1})+\underbrace{\left.\frac{\partial f}{\partial x}\right|_{\hat{x}_{k-1|k-1}}}_{=:\,F_k}\underbrace{(x_{k-1}-\hat{x}_{k-1|k-1})}_{=:\,\delta_{k-1|k-1}} \\
&= f(\hat{x}_{k-1|k-1}, u_{k-1})+F_k\,\delta_{k-1|k-1} \qquad \tag{B}
\end{align}
$$

Therefore, combining equation A and B we get:

$$
\begin{align}
\delta_{k|k-1} &= f(x_{k-1}, u_{k-1}) + w_{k-1} - f(\hat{x}_{k-1|k-1}, u_{k-1}) \\
&\approx \cancel{f(\hat{x}_{k-1|k-1}, u_{k-1})}+F_k\,\delta_{k-1|k-1} + w_{k-1} - \cancel{f(\hat{x}_{k-1|k-1}, u_{k-1})} \\
&= F_k\,\delta_{k-1|k-1}+w_{k-1} \tag{C}
\end{align}
$$

Define $P_{k|k-1} := \operatorname{Cov}(\delta_{k|k-1})$. By Huygen's identity:

$$
P_{k|k} = \operatorname{Cov}(\delta_{k|k}) = \mathbb{E}[\delta_{k|k}\,\delta_{k|k}^\top] - \mathbb{E}[\delta_{k|k}]\,\mathbb{E}[\delta_{k|k}]^\top
$$

For an unbiased estimator, $E[\delta_{k\mid k}]=0$, so:

$$
\begin{align}
P_{k|k-1}&=\mathbb{E}\left[\delta_{k|k-1}\,\delta_{k|k-1}^\top\right] \\
&=\mathbb{E}\left[( F_k\,\delta_{k-1|k-1}+w_{k-1})( F_k\,\delta_{k-1|k-1}+w_{k-1})^\top\right] \qquad\text{(using eq'n C)}\\
&= \mathbb{E}\left[F_k\,\delta_{k-1|k-1}\,\delta_{k-1|k-1}^\top F_k^\top + F_k\,\delta_{k-1|k-1}\,w_{k-1}^\top + w_{k-1}\,\delta_{k-1|k-1}^\top\,F_k^\top +w_{k-1}\,w_{k-1}^\top \right] \\
&= F_k\,\mathbb{E}\left[\delta_{k-1|k-1}\,\delta_{k-1|k-1}^\top\right] F_k^\top + F_k\,\mathbb{E}\left[\delta_{k-1|k-1}\,w_{k-1}^\top\right] + \mathbb{E}\left[w_{k-1}\,\delta_{k-1|k-1}^\top\right]F_k^\top + \mathbb{E}\left[w_{k-1}\,w_{k-1}^\top\right]
\end{align}
$$

Assuming that the process noise $w_{k-1}$ and previous posterior prediction error $\delta_{k-1|k-1}$ is uncorrelated:

$$
\begin{gather}
\mathbb{E}\left[\delta_{k-1|k-1}\,w_{k-1}^\top\right] = 0 \\
\mathbb{E}\left[w_{k-1}\,\delta_{k-1|k-1}^\top\right] = 0
\end{gather}
$$

Therefore:

$$
\begin{gather}
\Rightarrow\quad P_{k|k-1}\approx F_k\,\underbrace{\mathbb{E}\left[\delta_{k-1|k-1}\,\delta_{k-1|k-1}^\top\right] }_{=:\,P_{k-1|k-1}}F_k^\top + \underbrace{\mathbb{E}\left[w_{k-1}\,w_{k-1}^\top\right]}_{=:\,Q_k} \\
\therefore \qquad\boxed{P_{k|k-1} =F_k\,P_{k-1|k-1}\,F_k^\top+Q_k} \tag{3}
\end{gather}
$$

## Update Step

### 1. Get most recent sensor data, $z_k$

### 2. Predict measurement, $\hat{z}_k$

First, we predict the measurement we should get from the current prior state prediction, $x_{k|k-1}$ .

$$
\boxed{\hat{z}_k:=h(\hat{x}_{k|k-1}, u_k)} \tag{4}
$$

### 3. Calculate the innovation (measurement residual), $y_k$

We subtract the true measurement $z_k$ from the predicted measurement $\hat{z}_k$ to get the what is known as the "innovation" .

$$
\boxed{y_k := z_k-\hat{z}_k} \tag{5}
$$

A positive innovation indicates that the actual measurement is greater than the predicted measurement, and vice versa.  

### 4. Predict the innovation covariance, $S_k$

$$
\begin{align}
y_k &= z_k - \hat{z}_k \\
&= h(x_k, u_k) + v_k - h(\hat{x}_{k|k-1}, u_k) \qquad\text{(using eq'n 1 and 4)} \tag{D}
\end{align}
$$

Linearize $h$ about $\hat{x}_{k|k-1}$:

$$
\begin{align}
h(x_k, u_k) &\approx h(\hat{x}_{k|k-1}, u_k) + \underbrace{\left.\frac{\partial h}{\partial x}\right|_{\hat{x}_{k|k-1}}}_{=:\,H_k}\underbrace{(x_{k}-\hat{x}_{k|k-1})}_{=\,\delta_{k|k-1}} \\
&= h(\hat{x}_{k|k-1}, u_k) + H_k\,\delta_{k|k-1} \tag{E}
\end{align}
$$

Therefore, combining equation D and E, we get:

$$
\begin{align}
y_k &= h(x_k, u_k) + v_k - h(\hat{x}_{k|k-1}, u_k) \\
&\approx \cancel{h(\hat{x}_{k|k-1}, u_k)} + H_k\,\delta_{k|k-1} + v_k - \cancel{h(\hat{x}_{k|k-1}, u_k)} \\
&= H_k\,\delta_{k|k-1} + v_k \tag{F}
\end{align}
$$

Define $S_k:=\operatorname{Cov}(y_k)$ . Similarly, we assume unbiased estimator, so take $E[y_k]=0$.

$$
\begin{align}
S_k &= \mathbb{E}\left[y_k\,y_k^\top\right] \\
&= \mathbb{E}\left[(H_k\,\delta_{k|k-1}+v_k)(H_k\,\delta_{k|k-1}+v_k)^\top \right] \\
&= \mathbb{E}\left[H_k\,\delta_{k|k-1}\,\delta_{k|k-1}^\top\,H_k^\top + H_k\,\delta_{k|k-1}\,v_k^\top + v_k\,\delta_{k|k-1}^\top\,H_k^\top + v_k\,v_k^\top \right] \\
&= H_k\,\mathbb{E}\left[\delta_{k|k-1}\,\delta_{k|k-1}^\top\right]\,H_k^\top + H_k\,\mathbb{E}\left[\delta_{k|k-1}\,v_k^\top\right] + \mathbb{E}\left[v_k\,\delta_{k|k-1}^\top\right]\,H_k^\top + \mathbb{E}\left[v_k\,v_k^\top\right]
\end{align}
$$

Assuming that the measurement noise $v_k$ and the prediction error $\delta_{k|k-1}$ is uncorrelated, so:

$$
\begin{gather}
\mathbb{E}\left[\delta_{k|k-1}\,v_k^\top\right] = 0 \\ \mathbb{E}\left[v_k\,\delta_{k|k-1}^\top\right] = 0
\end{gather}
$$

Therefore:

$$
\begin{gather}
\Rightarrow \quad S_k \approx H_k\,\underbrace{\mathbb{E}\left[\delta_{k|k-1}\,\delta_{k|k-1}^\top\right]}_{=:\,P_{k|k-1}}\,H_k^\top + \underbrace{\mathbb{E}\left[v_k\,v_k^\top\right]}_{=:\,R_k} \\
\therefore \quad  \boxed{S_k=H_k\,P_{k|k-1}\,H_k^\top + R_k}  \tag{6}
\end{gather}
$$

$H_k\,P_{k|k-1}\,H_k^\top$ is the predicted measurement uncertainty caused by uncertainty in the state. $R_k$ accounts for the sensor’s measurement noise.
Now, our goal is to adjust prediction to get new predicted stated, $\hat{x}_{k|k}$

### 5. Adjusting the predicted posterior estimation-error covariance based on innovation, $P_{k|k}$**

We now aim to use the innovation $y_k$ to adjust our prediction $\hat{x}_{k|k-1}$ to get a new prediction of the state $\hat{x}_{k|k}$ . This takes the form:

$$
\boxed{\hat{x}_{k|k}:=\hat{x}_{k|k-1}+K_k\,y_k} \tag{7}
$$

where $K_k$ is the Kalman gain. The Kalman gain determines how much the filter changes its predicted state in response to the innovation.

We aim to find the Kalman gain which minimizes the _expected squared posterior estimation error_ $\delta_{k|k} := x_k - \hat{x}_{k|k}$. For an unbiased estimator,  $E[\delta_{k\mid k}]=0$ so the posterior estimation-error covariance is equal to the mean-square-error matrix:

$$
P_{k|k}=E[\delta_{k|k}\,\delta_{k| k}^{\top}]
$$

Furthermore, $E[|\delta_{k|k}|^2]=\tr(P_{k|k})$. Therefore, minimizing the expected squared estimation error is equivalent to minimizing the trace of the posterior estimation-error covariance.

For a linear system with Gaussian, correctly modelled noise, this solution is optimal. For an EKF, it is only locally approximately optimal because the nonlinear models have been linearized.

Combining equation F and G, we get:

$$
\hat{x}_{k|k}=\hat{x}_{k|k-1}+K_k\,(H_k\,\delta_{k|k-1}+v_k)
$$

Taking the negative of the above equation and adding it to $x_k$, we get:

$$
\underbrace{x_k - \hat{x}_{k|k}}_{=\,\delta_{k|k}} = \underbrace{x_k - \hat{x}_{k|k-1}}_{=\,\delta_{k|k-1}} - K_k\,(H_k\,\delta_{k|k-1}+v_k) \\
$$

$$
\begin{align}
\therefore \quad \delta_{k|k} &= \delta_{k|k-1}-K_k\,(H_k\,\delta_{k|k-1}+v_k) \\
&= (I-K_k\,H_k)\,\delta_{k|k-1} - K_k\,v_k \tag{G}
\end{align}
$$

Using the above, we aim to derive an expression which expressed the posterior covariance $P_{k|k} := \operatorname{Cov}(\delta_{k|k})$ in terms of an arbitrary gain $K_k$. We then choose $k_k$ to minimize the resulting expected squared error.

Assuming that the estimator is assumed to be unbiased, Huygen's identity gives us that:

$$
\begin{align}
P_{k|k} &= \mathbb{E}\left[\delta_{k|k}\delta_{k|k}^\top\right] \\
&= \mathbb{E}\left[\left((I-K_k\,H_k)\,\delta_{k|k-1} - K_k\,v_k \right)\left((I-K_k\,H_k)\,\delta_{k|k-1} - K_k\,v_k \right)^\top\right] \\
&= \mathbb{E}\left[(I-K_k\,H_k)\,\delta_{k|k-1}\,\delta_{k|k-1}^\top\,(I-K_k\,H_k)^\top - (I-K_k\,H_k)\,\delta_{k|k-1}\,v_k^\top K_k^\top - K_k\,v_k\,\delta_{k|k-1}^\top\,(I-K_k\,H_k)^\top + K_k\,v_k\,v_k^\top\,K_k^\top\right] \\
&=(I-K_k\,H_k)\,\mathbb{E}\left[\delta_{k|k-1}\,\delta_{k|k-1}^\top\right]\,(I-K_k\,H_k)^\top - (I-K_k\,H_k)\,\mathbb{E}\left[\delta_{k|k-1}\,v_k^\top\right] K_k^\top - K_k\,\mathbb{E}\left[v_k\,\delta_{k|k-1}^\top\right]\,(I-K_k\,H_k)^\top + K_k\,\mathbb{E}\left[v_k\,v_k^\top\right]\,K_k^\top
\end{align}
$$

Assuming that the measurement noise $v_k$ and the prediction error $\delta_{k|k}$ is uncorrelated, so:

$$
\begin{gather}
\mathbb{E}\left[\delta_{k|k-1}\,v_k^\top\right] = 0\\
\mathbb{E}\left[v_k\,\delta_{k|k-1}^\top\right] = 0
\end{gather}
$$

Therefore:

$$
\Rightarrow \qquad P_{k|k} \approx (I-K_k\,H_k)\,\underbrace{\mathbb{E}\left[\delta_{k|k-1}\,\delta_{k|k-1}^\top\right]}_{=\,P_{k|k-1}}\,(I-K_k\,H_k)^\top + K_k\,\underbrace{\mathbb{E}\left[v_k\,v_k^\top\right]}_{=\,R_k}\,K_k^\top
$$

$$
\therefore \quad \boxed{P_{k|k}= (I-K_k\,H_k)\,P_{k|k-1}\,(I-K_k\,H_k)^\top + K_k\,R_k\,K_k^\top} \tag{8a}
$$

This form is of the state-estimation covariance update is known as **Joseph form**. There is a simpler, sometimes known as the **simplified covariance update**.

$$
\boxed{P_{k|k}=(I-K_k\,H_k)\,P_{k|k-1}} \tag{8b}
$$

### 6. Find the Kalman gain, $K_k :$

Expanding equation 8, we get:

$$
\begin{align}
P_{k|k}&= P_{k|k-1} - K_k\,H_k\,P_{k|k-1}-P_{k|
k-1}\,H_k^\top\,K_k^\top + K_k\,\underbrace{\left(H_k\,P_{k|k-1}\,H_k^\top+R_k\right)}_{=\,S_k}\,K_k^\top \\
&= P_{k|k-1} - K_k\,H_k\,P_{k|k-1}-P_{k|
k-1}\,H_k^\top\,K_k^\top +K_k\,S_k\,K_k^\top \tag{H}
\end{align}
$$

Define $K_k^*$ to be the optimal Kalman gain. As previously mentioned in step 5, minimizing the expected squared estimation error is equivalent to minimizing the trace of the posterior estimation-error covariance. The trace is the sum of the individual state-error variances. For zero-mean errors, it equals the expected squared Euclidean norm of the estimation error.

$$
\tr(P_{k|k})=\mathbb{E}\left[\lVert\delta_{k|k}\rVert^2\right]
$$

Therefore:

$$
K_k^*= \underset{K_k}{\arg\min}\;\tr\!\left(P_{k|k}(K_k)\right)
$$

For brevity, the following identities will be used without proof in the derivation:

<ol type="a" markdown="1">
  <li markdown="1">$\tr(A)=\tr(A^\top)$</li>
  <li markdown="1">$\tr(AB)=\tr(BA)$</li>
  <li markdown="1">$\frac{\partial}{\partial X}\!\left(\tr(XA)\right) = A^\top$</li>
  <li markdown="1">$\frac{\partial}{\partial X}\!\left(\tr(XAX^\top)\right) = 2XA \qquad$ for symmetrical A. </li>
  <li markdown="1">$A=A^\top\qquad$ for symmetrical A</li>
</ol>

Now the fun begins. Taking the traces of both sides of equation H, we get that:

$$
\tr\left(P_{k|k}(K_k)\right) = \tr(P_{k|k-1}) - \tr(K_k\,H_k\,P_{k|k-1})-\underbrace{\tr(P_{k|
k-1}\,H_k^\top\,K_k^\top)}_{(*)} + \tr(K_k\,S_k\,K_k^\top)
$$

Using the symmetry of the covariance matrix $P_{k|k-1}$:

$$
\begin{align}
\text{(*)} &=\tr(P_{k|
k-1}\,H_k^\top\,K_k^\top) \\
&= \tr\left((K_k\,H_k\,P_{k|k-1}^\top)^\top\right) \\
&= \tr\left(K_k\,H_k\,P_{k|k-1}^\top\right)\qquad\qquad\text{(by identity (a))} \\
&= \tr\left(K_k\,H_k\,P_{k|k-1}\right)\qquad\qquad\text{(by symmetry of covariance)}
\end{align}
$$

$$
\therefore \qquad \tr\left(P_{k|k}(K_k)\right) = \tr(P_{k|k-1}) - 2\cdot\tr(K_k\,H_k\,P_{k|k-1}) + \tr(K_k\,S_k\,K_k^\top) \tag{I}
$$

To find the minima, we take the derivative of equation I and set it to zero. Using identities c and d:

$$
\begin{gather}
\frac{\partial}{\partial K_k}\left(\tr\left(P_{k|k}(K_k)\right)\right)= -2(H_k\,P_{k|k-1})^\top+2K_k\,S_k \overset{\mathrm{set}}{=} 0 \\
\Rightarrow K_k\,S_k=P_{k|k-1}\,H_k^\top \\
\therefore \quad \boxed{K_k = P_{k|k-1}\,H_k^\top S_k^{-1}} \tag{9}
\end{gather}
$$

Note the matrix inverse on $S_k$. In general, we avoid directly computing the inverse because it is less numerically stable and also requires more compute. Instead, we can exploit the positive semi-definiteness of $S_k$ and use **Cholesky factorization** to find $K_k$ directly. CMSIS-DSP has implementations of Cholesky which make it very easy to find $L$.

### 7. Update the state prediction:**

Therefore, substituting equation 9 into equation 7:

$$
\boxed{\hat{x}_{k|k}= \hat{x}_{k|k-1}+(P_{k|k-1}\,H_k^\top\,S_k^{-1})\,y_k} \tag{10}
$$

## Appendix A: Deriving Simplified Covariance Update

Starting from the Joseph Form:

$$
\begin{align}
P_{k|k} &= (I-K_kH_k)\,P_{k|k-1}\,(I-K_k\,H_k)^\top + K_k\,R_k\,K_k^\top \\
&= P_{k|k-1}-K_k\,H_k\,P_{k|k-1}-P_{k|k-1}\,H_k^\top\,K_k^\top+K_k\,H_k\,P_{k|k-1}\,H_k^\top\,K_k^\top+ K_k\,R_k\,K_k^\top \\
&= P_{k|k-1}-K_k\,H_k\,P_{k|k-1}-P_{k|k-1}\,H_k^\top\,K_k^\top+K_k\,\underbrace{(H_k\,P_{k|k-1}\,H_k^\top + R_k)}_{=\,S_k}\,K_k^\top \\
&= P_{k|k-1}-K_k\,H_k\,P_{k|k-1}-P_{k|k-1}\,H_k^\top\,K_k^\top+K_k\,S_k\,K_k^\top \tag{J}
\end{align}
$$

Assuming optimal Kalman gain $K_k =P_{k|k-1}\,H_k^\top\,S_k^{-1}$:

$$
\Rightarrow \quad K_k\,S_k = P_{k|k-1}\,H_k^\top\tag{K}
$$

Substituting equation K into equation J:

$$
\begin{gather}
\therefore \quad P_{k|k} = P_{k|k-1}-K_k\,H_k\,P_{k|k-1}-\cancel{P_{k|k-1}\,H_k^\top\,K_k^\top}+\cancel{P_{k|k-1}H_k\,K_k^\top} \\
\boxed{P_{k|k} = (I-K_k\,H_k)\,P_{k|k-1}}
\end{gather}
$$

## Appendix B: Use Cholesky factorization to find Kalman Gain

Starting with equation K:

$$
\begin{gather}
K_k\,S_k = P_{k|k-1}\,H_k^\top \\
\Rightarrow (K_k\,S_k)^\top = (P_{k|k-1}\,H_k^\top)^\top \\
\Rightarrow S_k^\top K_k^\top = H_k\,P_{k|k-1}^\top \\
\Rightarrow S_k\,K_k^\top = H_k\,P_{k|k-1} \tag{L}
\end{gather}
$$

Since $S_k$ is positive definite, we can decompose $S_k := LL^\top$, where $L$ is lower triangular, so $L^\top$ is upper triangular.

Let $X := K_k^\top$ and $B:=H_k\,P_{k|k-1}$. Then equation L becomes:

$$
LL^\top X=B
$$

Take $Y = L^\top X$. Then by the above, $LY = B$.  Now, to find $K_k$, we do the following steps:

1. Solve $LY=B$ using forward substitution.
2. Solve $L^\top X = Y$ using backward subtitution.
3. $K_k=X^\top$.

Voila.
