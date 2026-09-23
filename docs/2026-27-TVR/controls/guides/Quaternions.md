Define the vector space $\mathbb H := \mathbb R \times \mathbb R^3$ as the real vector space with basis $\set{1, \hat i, \hat j, \hat k}$ to be the set of all quaternions, i.e.

$$
\mathbb H = \set{a + b\hat i + c\hat j + d\hat k \mid a, b, c, d\in\mathbb{R}}
$$

Multiplication is associative and is extended bilinearly over $\mathbb R$. The basis of $\mathbb H$ satisfies the following multiplicative identities:

$$
\hat i^2 = \hat j^2 = \hat k^2=\hat i\hat j\hat k = -1
$$

From the definition above, we can derive the following (notice the noncommutativity):

$$
\begin{gather}
\hat i\hat j = -\hat j\hat i = \hat k\\
\hat j\hat k = -\hat k\hat j = \hat i\\
\hat k\hat i = -\hat i\hat k = \hat j\\
\end{gather}
$$

This fully defines the quaternions. 

Quaternions are usually written in the form of $p = a+b\hat i+c\hat j + d\hat k$, though they are sometimes also written in a form called scalar-vector notation:  $p = (q_0, \mathbf q)$ where $q_0 \in\mathbb R$ and $\mathbf{q}\in\mathbb{R}^3$.

## Hamilton Product (Quaternion Multiplication)

I now aim to define the product of two arbitrary quaternions. 

Let $p = a + \mathbf{u}$ and $q = b + \mathbf{v}$, where:

$$
\mathbf{u} = u_x\hat i + u_y\hat j + u_z\hat k,\qquad \mathbf{v} = v_x\hat i + v_y\hat j + v_z\hat k
$$

By bilinearity:

$$
pq = ab + a\mathbf{v} + b\mathbf{u} + \mathbf{uv}
$$

Expanding the latter term, we get:

$$
\begin{align}
\mathbf{uv} &= (u_x\hat i + u_y\hat j + u_z\hat k)(v_x\hat i + v_y\hat j + v_z\hat k) \\
&= -(u_xv_x+u_yv_y+u_zv_z) + (u_yv_z-u_zv_y)\hat i + (u_zv_x - u_xv_z)\hat j+(u_xv_y - u_yv_x)\hat k \\
&= -\mathbf{u}\cdot\mathbf{v} + \mathbf{u}\times\mathbf{v}
\end{align}
$$

Therefore:

$$
\boxed{pq = \left(ab-\mathbf{u\cdot v}\,,\, a\mathbf{v} + b\mathbf{u}+\mathbf{u\times v}\right)}
$$

## Quaternion Operations

Let $p = a + b\hat i+ c\hat j + d\hat k =(a, \mathbf{v})$ be a quaternion. 

### Quaternion Conjugate

The quaternion conjugate of $p$ is obtained by negating the vector part

$$
p^* = (a, -\mathbf{v})
$$

Multiplying a quaternion by its conjugate yields its magnitude squared:

$$
pp^*=p^*p= a^2+b^2 +c^2+d^2=\lVert p\rVert^2
$$

### Quaternion Inverse

The quaternion inverse of $p$ satisfies the following identity:

$$
pp^{-1} =p^{-1}p = 1
$$

Since $pp^* = p^*p = \lVert p\rVert^2$, we therefore have that:

$$
p^{-1} = \frac{p^*}{\lVert p\rVert^2}
$$

Therefore, if $p$ is unitary, the inverse and the conjugate are equal.

### Quaternion Sandwich Product

$$
\mathbf{v'} =  q\mathbf{v} q^{-1}
$$

Suppose we have a vector $\mathbf v \in \mathbb R^3$ that is represented as a pure quaternion $v = (0, \mathbf{v})$. 

Let:

$$
\hat q(\theta, \mathbf{\hat n}) = \left(\cos\left(\frac\theta 2\right), \mathbf{\hat n}\sin\left(\frac\theta 2\right)\right)
$$

be a unit quaternion, where $\mathbf{\hat n}$ is the unit vector along the rotation axis, and $\theta$ is the rotation angle along that axis (following RHR conventions). 

We will now show that the sandwich product:

$$
\mathbf{v'} = \hat q\mathbf{v}\hat q^{-1} =\hat q\mathbf{v}\hat q^* 
$$

rotates the vector $\mathbf{v}$ about the rotation axis $\mathbf{\hat n}$ with an angle of $\theta$. 

_Proof_:
Let $c = \cos\frac\theta 2$ and $s=\sin\frac\theta 2$. Then $\hat q = (c, s\mathbf{\hat n})$. By the Hamilton product

$$
\begin{align}
\hat q v &= (c, s\mathbf{\hat n})(0, \mathbf{v}) \\
&= (-s\mathbf{\hat n\cdot\mathbf{v}}, c\mathbf{v}+s\mathbf{\hat n}\times\mathbf{v})
\end{align}
$$

Right-multiplying the above by $\hat q^{-1} = \hat q^*$, we get that:

$$
\begin{align}
\hat q v \hat q^{-1} &= \hat q v \hat q^* \\
&= (-s\mathbf{\hat n\cdot\mathbf{v}}, c\mathbf{v}+s\mathbf{\hat n}\times\mathbf{v})(c, -s\mathbf{\hat n}) \\
&= (\underbrace{-sc(\mathbf{\hat n \cdot v})+sc(\mathbf{\hat n \cdot v})+s^2(\mathbf{\hat n \times v})\cdot \mathbf{\hat n}}_{=0}, s^2(\mathbf{\hat n\cdot v})\mathbf{\hat n} +c^2\mathbf{v}+sc(\mathbf{\hat n \times v})-(c\mathbf{v}+s\mathbf{\hat n}\times\mathbf{v})\times s\mathbf{\hat n}) \\
&= (0, s^2(\mathbf{\hat n\cdot v})\mathbf{\hat n} +c^2\mathbf{v}+sc(\mathbf{\hat n \times v})-sc(\mathbf{v\times\hat n}) -s^2(\mathbf{\hat n\times v})\times \mathbf{\hat n})
\end{align}
$$

Notice that the scalar part of the above is zero, so it is a pure quaternion. Using the identities $\mathbf{a\times b} = -\mathbf{b \times a}$ and $(\mathbf{a \times b}) \times \mathbf{c} = \mathbf{b}(\mathbf{a \cdot c}) - \mathbf{a}(\mathbf{b\cdot c})$, we get:

$$
\begin{align}
\mathbf{v'} &= s^2(\mathbf{\hat n\cdot v})\mathbf{\hat n} +c^2\mathbf{v}+2sc(\mathbf{\hat n \times v}) -s^2(\mathbf{v}(\mathbf{\hat n\cdot\hat n})-\mathbf{\hat n}(\mathbf{v\cdot \hat n})) \\
&= (c^2-s^2)\mathbf{v} +2sc(\mathbf{\hat n \times v})+2s^2(\mathbf{\hat n \cdot v})\mathbf{\hat n}
\end{align}
$$

Finally, applying the double angle identities, we get that:

$$
\mathbf{v'} = \mathbf{v}\cos\theta + (\mathbf{\hat n \times v})\sin\theta + \mathbf{\hat n}(\mathbf{\hat n\cdot v})(1-\cos\theta)
$$

We note that this is identically the Rodrigues' rotation formula. 

If you are not convinced, let us prove that this indeed does what it says. Take:

$$
\mathbf{v}_\Vert = \mathbf(\mathbf{\hat n\cdot v})\mathbf{\hat n}, \qquad \mathbf{v_\perp=v-v_\Vert}
$$

Then:

$$
\begin{align}
\mathbf{v'} &= \mathbf{v}\cos\theta + (\mathbf{\hat n \times v})\sin\theta + \mathbf{v_\Vert}(1-\cos\theta) \\
&= \mathbf{v_\Vert+(v-v_\Vert)}\cos\theta+ (\mathbf{\hat n \times v})\sin\theta \\
&= \mathbf{v_\Vert+v_\perp}\cos\theta+(\mathbf{\hat n\times (v_\Vert+v_\perp)})\sin\theta \\
&=\mathbf{v_\Vert+v_\perp}\cos\theta+(\mathbf{\hat n\times v_\perp})\sin\theta
\end{align}
$$

Notice that $\mathbf{v_\Vert}$ remains unchanged by the operation. If $\mathbf{v}_\perp \neq 0$, then the unit vectors:

$$
\mathbf{\hat e_1} = \mathbf{\frac{v_\perp}{\lVert v_\perp \rVert}},\qquad \mathbf{\hat e_2} = \mathbf{\hat n\times \hat e_1}
$$

forms a orthonormal basis on the plane perpendicular to $\mathbf{\hat n}$. A rotation of $\theta$ on the vector $\mathbf{v_\perp}$ on this plane looks like:

$$
R_\mathbf{\hat n}(\theta)\mathbf{v_\perp} = \mathbf{v_\perp}(\mathbf{\hat e_1}\cos\theta+\mathbf{\hat e_2}\sin\theta)
$$

which is precisely a rotation of $\theta$ using RHR. Therefore the quaternion sandwich product indeed does rotation. 

Q.E.D.

## Left-Multiply vs. Right-Multiply

Let $q$ be the **rocket’s orientation**, mapping body coordinates to world coordinates. For an additional rotation $\delta q$:

- **World-frame rotation:** $q'=\delta q q$ -- rotate in the world frame. Intuition: Rotate about a rocket axis first, then let $q$ carry that axis into the rocket’s current orientation. So the rotation follows the **body axis**.
- **Body-frame rotation:** $q'=q \delta q$ -- rotate about a body frame. Intuition: Place the rocket in its current orientation first, then rotate it about a fixed **world axis**.

Here, $\delta q$ axis is expressed in the corresponding frame.

**World → multiply on the left. Body → multiply on the right.**

## Why Quaternions?

We use quaternions because it give you a compact way to represent the rocket’s orientation that stays well behaved as it rotates.  Euler angles can be become singular at certain angles (known as Gimbal Lock). If two of the rotation axes align, and the angles stop describing three independent rotational directions. It is possible to choose a convention that avoids it around upright flight, but quaternions avoid orientation singularities altogether. It also makes the math a lot more convenient. 
