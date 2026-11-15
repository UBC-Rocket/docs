
$$
\begin{gather}
\boldsymbol{a}_{nom} = \boldsymbol{a}_{meas}-\boldsymbol{\beta}_a \\
\boldsymbol{\omega}_{nom} = \boldsymbol{\omega}_{meas} - \boldsymbol{\beta}_\omega
\end{gather}
$$

$$
\boldsymbol{a}_W = \mathrm{R}(q)\boldsymbol{a}_{nom} + \boldsymbol{g}
$$

## Nominal State Update Equations

$$
\begin{align}
\boldsymbol{p} &\leftarrow \boldsymbol{p} + \boldsymbol{v}\Delta t + \frac{1}{2}\boldsymbol{a}_W\Delta t^2 \\
\boldsymbol{v} &\leftarrow \boldsymbol{v} + \boldsymbol{a}_W\Delta t \\
\boldsymbol{q} &\leftarrow \mathbf{q}\,\otimes\,\mathrm{Exp_q}(\boldsymbol{\omega}_{norm}\Delta t) \\
\boldsymbol{\beta}_a &\leftarrow\boldsymbol{\beta}_a \\
\boldsymbol{\beta}_\omega &\leftarrow\boldsymbol{\beta}_\omega \\
\boldsymbol{\beta}_{baro} &\leftarrow\boldsymbol{\beta}_{baro}
\end{align}
$$

Remember to normalize the quaternion every nominal state update.

## Error State Update Equations

$$
\delta\boldsymbol{a}_W =  - \mathrm{R}(q)\left([\boldsymbol{a}_{nom}]_\times \delta\boldsymbol{\theta}+\delta\boldsymbol{\beta}_a\right)
$$

$$
\begin{align}
\delta\boldsymbol{p} &\leftarrow \delta\boldsymbol{p} + \delta\boldsymbol{v}\Delta t  + \frac{1}{2}\delta\boldsymbol{a}_W\Delta t^2 \\
\delta\boldsymbol{v} &\leftarrow \delta\boldsymbol{v} + \delta\boldsymbol{a}_W \Delta t \\
\delta\boldsymbol{\theta} &\leftarrow \exp(-[\boldsymbol{\omega}_{nom}\Delta t]_\times)\delta \boldsymbol{\theta} - J_r(\boldsymbol{\omega}_{nom}\Delta t)\delta\boldsymbol{\beta}_\omega \Delta t \\
\delta\boldsymbol{\beta}_a &\leftarrow \delta\boldsymbol{\beta}_a \\
\delta\boldsymbol{\beta}_\omega &\leftarrow \delta\boldsymbol{\beta}_\omega \\
\delta\boldsymbol{\beta}_{baro} &\leftarrow \delta\boldsymbol{\beta}_{baro}
\end{align}
$$
