$n$ people stand in a circle waiting to be executed. Counting begins at a specified point in the circle from $1$. After $k−1$ people are skipped, the $k$ th person get executed. Then starting with next person from $1$, after $k−1$ people are skipped, the $k$ th person get executed. It is going on until the last one remained. Let the ${\rm Josephus}(n,k),n,k\in \mathbb{Z}_+$ denote the position of the survivor when there are $n$ people and $1$ is the start point of first round.<br>

if $k=2$ and $n$ is even:<br>
$${\rm Josephus}(n,2)=2{\rm Josephus}\left(\frac{n}{2},2\right)-1$$
if $k=2$ and $n$ is odd:<br>
$${\rm Josephus}(n,2)=2{\rm Josephus}\left(\frac{n-1}{2},2\right)+1$$
if $k=2,n=2^m+l,m,l\in \mathbb{Z}_+,0\leq l\leq 2^m$:<br>
$${\rm Josephus}(n,2)=2l+1=2\left(n-2^{\lfloor \log_2 n\rfloor}\right)+1$$

if $k\geq 2$, we can get ${\rm Josephus}(1,k)=1$, and:<br>
$${\rm Josephus}(n-1,k)=\begin{cases}
{\rm Josephus}(n,k)+n-k & \text{if }{\rm Josephus}(n,k)\leq k\\
{\rm Josephus}(n,k)-k & \text{if }{\rm Josephus}(n,k)>k \end{cases}$$
$${\rm Josephus}(n,k)=({\rm Josephus}(n-1,k)k+1)\bmod n+1$$
