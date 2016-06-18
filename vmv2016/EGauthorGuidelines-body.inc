% ---------------------------------------------------------------------
% EG author guidelines plus sample file for EG publication using LaTeX2e input
% D.Fellner, v1.17, Sep 23, 2010

  
\usepackage{amsmath}
\title[3D Face Reconstruction with Silhouette Constraints]%
      {3D Face Reconstruction with Silhouette Constraints}

% for anonymous conference submission please enter your SUBMISSION ID
% instead of the author's name (and leave the affiliation blank) !!
%\author[D. Fellner \& S. Behnke]
       %{D.\,W. Fellner\thanks{Chairman Eurographics Publications Board}$^{1,2}$
%        and S. Behnke$^{2}$
%        S. Spencer$^2$\thanks{Chairman Siggraph Publications Board}
%        \\
% For Computer Graphics Forum: Please use the abbreviation of your first name.
         %$^1$TU Darmstadt \& Fraunhofer IGD, Germany\\
%         $^2$Institut f{\"u}r ComputerGraphik \& Wissensvisualisierung, TU Graz, Austria
%        $^2$ Another Department to illustrate the use in papers from authors
%             with different affiliations
%       }
\author{Anonymous}

% ------------------------------------------------------------------------

% if the Editors-in-Chief have given you the data, you may uncomment
% the following five lines and insert it here
%
% \volume{27}   % the volume in which the issue will be published;
% \issue{1}     % the issue number of the publication
% \pStartPage{1}      % set starting page


%-------------------------------------------------------------------------
\begin{document}

\teaser{
  \includegraphics[width=\linewidth]{figures/teaser.pdf}
  \centering
   \caption{We introduce silhouette constraints to improve the quality of unconstrained 3D face reconstruction. Here, we show a comparison between the state of the art approach by Roth et al. \cite{Roth:2015:UFR} and our technique. Note how our approach is more faithful to the silhouettes of the input images, especially around the nose region on the right.}
 \label{fig:teaser}
}

\maketitle

\begin{abstract}
In this paper we introduce silhouette constraints to improve the quality of unconstrained 3D face reconstruction. Previously, state of the art unconstrained 3D face reconstruction techniques relied solely on photometric consistency and matching sparse facial landmarks. We show that constraining the silhouettes of the 3D reconstruction to match silhouettes in the input images can further improve reconstruction quality. Our technique automatically detects silhouettes, and iteratively matches silhouette points computed from the current 3D reconstruction with silhouettes in the input images. We demonstrate that our results improve on the previous state of the art in unconstrained 3D face reconstruction, and that our additional constraints can easily be included in the iterative reconstruction at little additional cost.
%\begin{classification} % according to http://www.acm.org/class/1998/
%\CCScat{Computer Graphics}{I.3.3}{Picture/Image Generation}{Line and curve generation}
%\end{classification}

\end{abstract}


%-------------------------------------------------------------------------
\section{Introduction}

We consider the problem of 3D face reconstruction from internet photo collections. Our goal is to reconstruct 3D models of individuals from collections of images in uncontrolled environments, including variations in illumination, pose, and expression, which has been called ``face reconstruction in the wild'' \cite{Kemelmacher-Shlizerman:2011:FRW} or ``unconstrained 3D face reconstruction'' \cite{Roth:2015:UFR}. Such 3D reconstructions can be useful for face and expression recognition \cite{Liu:2005:PFR,Wang:2006:FER}, or to produce facial animations \cite{Li:2013:RFA}.

Our work is inspired by recent progress in unconstrained face recognition by Roth et al. \cite{Roth:2015:UFR}. They leverage state of the art photometric stereo techniques, recent advances in 2D facial landmark estimation, and a full 3D face representation (instead of 2.5D height fields) to obtain impressive results, considering that the input consists of images under various illumination conditions, with different poses and facial expressions, and neither video or stereo data is included in the input. Nonetheless, the quality of the 3D reconstructions is limited since the only constraints on the reconstruction are photometric consistency and correspondence with sparse facial landmarks. 

In this paper, we introduce silhouette constraints to improve the quality of unconstrained 3D face reconstruction. Our main idea is to extract silhouette points on the 3D reconstruction, and match them with automatically detected silhouette points in the input images. We include these constraints in the 3D reconstruction objective, which we solve in an iterative process. In each iteration step, we recompute the silhouette points using the current 3D reconstruction and update the corresponding contraints in the objective. As a consequence, the silhouettes of the 3D reconstruction converge towards the silhouettes in the input images. Our results demonstrate that the new silhouette constraints lead to higher reconstruction quality.

The rest of this paper is organized as follows: We first review related work in Section~\ref{sec:related}, and provide a brief overview of state of the art unconstrained face reconstruction as proposed by Roth et al.~\cite{Roth:2015:UFR} in Section~\ref{sec:unconstrainedfacereconstruction}. In Section~\ref{sec:silhouetteconstraints} we introduce the novel silhouette constraints as our main contribution. Finally, we present results in Section~\ref{sec:results} and conclude in Section~\ref{sec:conclusions}.


%-------------------------------------------------------------------------
\section{Related work}
\label{sec:related}

\paragraph*{Face Reconstruction from Image Collections} Face reconstruction ``in the wild'' from collections of images under uncontrolled illumination, and with varying facial pose and expressions, has been a long standing problem in computer vision. State of the art methods are mostly based on photometric stereo, such as the pioneering work by Kemelmacher-Shlizerman and Seitz \cite{Ira11}, and its extension to video input \cite{Suwajanakorn2014}. This approach has been improved recently by Roth et al.~\cite{Roth:2015:UFR} who solve for a full 3D mesh instead of a 2.5D height field, and leverage state of the art 2D facial landmark estimation \cite{Zan:2013:LCM}. We build on the work by Roth et al., but extend it to include silhouette constraints to overcome some of the limitations of photometric stereo.

\paragraph*{Face Tracking and Animation from Video} Research in the Computer Graphics community has achieved impressive results for the problem of tracking, reconstructing, and animating faces based on video data. Generally, state of the art techniques represent faces and facial expressions using dynamic expression models, also called blendshape models \cite{Cao:2014:FWA}. While earlier work relied on RGBD video input \cite{Weise:2011:RPF,Li:2013:RFA,Bouaziz:2013:OMR}, it is now possible to solve the tracking and reconstruction problem based on RGB video data in real-time at impressive quality as demonstrated by Cao et al. \cite{Cao:2014:DDE,Cao:2015:RHF}. The key idea in this work is to learn a regression model that maps 2D video frames to 3D facial landmarks, and then to register the DEM to the 3D landmarks \cite{Cao:2013:SRR}. While this approach required calibration for individual users, it has then been extended to a calibration-free approach that iteratively refines the model \cite{Cao:2014:DDE}. A most recent extension also synthesizes highly detailed geometry such as wrinkles \cite{Cao:2015:RHF}. The key difference to our work is that these techniques either require calibration for each user, or they rely on the coherence of video input to adapt the model in an iterative manner.


%-------------------------------------------------------------------------
\section{Unconstrained Face Reconstruction}
\label{sec:unconstrainedfacereconstruction}

Unconstrained 3D face reconstruction \cite{Roth15} takes as its input  a collection of facial photographs of an individual under uncrontrolled illumination and various poses and facial expresssions, and it reconstructs a full 3D reconstruction of the individual's face represented by a mesh. The method proceeds by first detecting 2D landmarks on all input images using the approach by Yan et al. \cite{Yan:2013:LCM}. Next, a 3D template mesh is deformed and projected to 2D to match the 2D landmarks. This leads to an initial, rough 3D reconstruction of the face, and a weak perspective projection matrix for each input image. Then they improve the 3D reconstruction by determining surface normals using a photometric stereo approach, and lastly they perform 3D reconstruction by refining the rough mesh to match the photometric normals. We briefly review the main components of this approach.

Given the 2D landmarks, denoted by a vector $W_i$, for all images $i$, and a template mesh with $p$ vertices, denoted by a $3p$ dimensional vector $X_0$, the goal is to find a deformed mesh $X$ and weak projection matrices $P_i$, such that the landmarks on $X$ projected to 2D match the 2D landmarks $W_i$. The key idea is to use Laplacian surface editing \cite{Sorkine:2004:LSE} to perform mesh deformation. This leads to a minimization problem
%
\begin{align}
\label{eq:warptemplate}
E_{warp}(X,P_{i})=\|\mathcal{L}X - \mathcal{L}X_{0}\|^2+\lambda_{l}\sum_{i}{\|P_{i}D_{i}X-W_{i}\|^2},
\end{align} 
%
which expresses the intuition that the mesh Laplacian \cite{Meyer2003} $\mathcal{L}X$ of the deformed mesh should stay close to the mesh Laplacian of the template $\mathcal{L}X_{0}$. In addition, $D_{i}$ is a selection matrix picking out the landmarks that have a correspondence in image $i$, that is, it is a diagonal matrix with $1$ on the diagonal for the vertices corresponding to a landmark and $0$ everywhere else. 

Equation~\ref{eq:warptemplate} is solved iteratively. First, the projection matrices are obtained by fixing the template and solving for $P_i$, which is a linear least squares problem. Then, the $P_i$ are fixed and the deformed mesh $X$ is obtained in an iterative approach. This is necessary because the mesh Laplacians $\mathcal{L}X$ and $\mathcal{L}X_0$ are not rotation invariant. Hence, after each iteration step the mesh Laplacian $\mathcal{L}X_0$ of the template is rotated to align with the Laplacian $\mathcal{L}X$ obtained in the previous step.

%First, in \cite{Roth15} they compute 2D landmarks for all images using the approach in [5]. With the 2D landmarks, Warp the 3D model on the images by project 3D landmarks on 2D landmarks on image. Estimate the weak prospecive projection matrix $P_{i}$ for each image. Deform the 3D template using laplacian surface editing and adapt for the lanmark constraints until convergence. Update the vertex verctor $X$ by minimizing the energy function:
%\begin{displaymath}
%E_{warp}(X,P_{i})=\|\mathcal{L}X - \mathcal{L}X_{0}\|^2+\lambda_{l}\sum_{i}{\|P_{i}D_{i}X-W_{i}\|^2} 
%\end{displaymath} 
%
%where $\mathcal{L}$ is a discretization of the Laplace-Beltrami operator, with the weight $\mathcal{L}_{ij}=\frac{1}{2}(cot\alpha_{ij}+cot\beta_{ij})$ where $\alpha_{ij}$ and $\beta_{ij}$ are the opposite angles of edge if in the two incident triangles. $D_{i}$ is the selection matrix picking out the landmarks that have a correspondence in image $i$, it's a diagonal matrix with 1 on the diagonal for the vertices corresponding to such a landmark and 0 everywhere else.
 %The deformed 3D template will be used as the initial template for the following steps.   

Then they use photometric stereo technique to estimate normals. Refer to approach in \cite{Ira11}, store each warped image reflectance intensity in a matrix $M$, where each row represents the pixels in one image. For the non-frontal images, some vertices are not visible. They set the intensity of the non-visible vertices as zeros and use matrix completion \cite{Lin09} to fulfill the missing value to obtain the full $M$. Factorize the matrix $M$ using SVD and take the rank-4 approximation to estimate the shape $S$ and light $L$, where $S$ contains the normals $n$ and albedo $\rho$, and $L$ contains the light directoin. Select the images that are modeled well by the rank-4 approximation, i.e.,$\|M-LS\|<\epsilon$.  Since the factorization is not unique, it leads to one ambiguity in $S$ and $L$, i.e., $LS=(\tilde{L}A^{-1})(A\tilde{S})$. Recover the ambiguity by solving for $\arg{\min}\underset{A}\,\|S^{t} - A\tilde{S}\|^2 $, where $S^{t}$ is the shape vector of the template. 

Given the normal vectors, they reconstruct the shape $X$ from the normals $n$ through mean curvature $H$ by minimizing $\|\mathcal{L}X-Hn\|^2$, where $H$ is the mean curvature. The mean curvature $H$ is estimated as 
$H_{i}=\frac{1}{4A_{i}} \sum_{j\in N(i)}{(cot\alpha_{ij}+cot\beta_{ij}))e_{ij}\cdot(n_{i}-n_{j})}$, where $N(i)$ is the set of incident neighboring vertices of vertex $i$, $A_{i}$ is the sum of the triangles' areas incident to $i$, $e_{ij}$ is the edge from $i$ to $j$. The cotan weights are the same as those in the Laplacian operator $\mathcal{L}$. Update the cotan weights in each global iteration.

On the boundary, the mean curvature formula degenerates into a 1D version, the shape vectore $X$ is updated by minimizing 
$\|\mathcal{L}_{b}X- \mathcal{K}b\|^2$, where $\mathcal{L}_{b,ij}=\frac{1}{e_{ij}}$ and $\mathcal{K}$ is the geodesic curvature along the boundary and $b$ is the cross product between the surface normal and the boundary tangent.

Finally, the mesh is deformed by minimizing the energy as following:
\begin{displaymath}
E=\|\mathcal{L}X - H^{k}n\|^2+\lambda_{b}\|\mathcal{L}_{b}X - \mathcal{L}X^{k}\|^2+  \lambda_{l}\sum_{i}{\|P_{i}D_{i}X-W_{i}\|^2} 
\end{displaymath} 

From the results shown in \cite{Roth15}, we can see that the frontal face of the 3D reconstructed model fits to the individual, but the profile doesn't fit well, especially the nose and chin.The nose is supposed to be taller and the chin should be more curved. So we propose to use silhouette of the faces to refine the reconstructed face shape.

%-------------------------------------------------------------------------
\section{Silhouette Constraints}
\label{sec:silhouetteconstraints}

Continue from the previous work, we refine the face model to make the shape fit better with the real image. We extract the silhouettes on 3D model and in each 2D images and build correpondence between the silhouette vertex on face model and the silhouette points in images. Then add silhouette constraint in the mesh deformation.

The main step can be described as following:

1. Reconstruct a 3D model using the method in \cite{Roth15}. 

2. Use photometric stereo to estimate the normal and compute the mean curvature using the propsed formula in \cite{Roth15}.

3. Estimate perspective projection matrix and rotation matrix. Extract 3D and 2D silhouette candidates for each image. Build up correspondence between 2D and 3D silhouette candidates. Discard the silhouette candidate points only show in few images.

4. Reconstruct face model with silhouette constraint.

5. Go back to step 2 until convergence. 

In the following subsection, we will give the details of step 2 to step 4.

%-------------------------------------------------------------------------
\subsection{Silhouette Extraction}


To extract silhouette on 3D model, first we detect the points on 3D mesh whose normal are parallel to the image plane. Given the estimated perspective projection matrix, we can estimate the rotation matrix $R_{i}$ for each image. The view direction then can be estimated from the rotation matrix. Suppose the direction perpendicular to the frontal face is z-axis, then for $i$-th image, view direction
$v_{i}=R_{i} \ast \left[ 0; 0; 1 \right]$. 
Find the silhouette candidate points on 3D model whose normals are perpendicular to the view direction, the cosine of the angle between view direction and normal should be near to zero, i.e, $\frac {|v_{i} \cdot n_{j|}} {\|v_{i} \| \cdot \|n_{j} \|} < \epsilon$, where $n_{j}$ is the normal of vertex $j$ on 3D mesh and $\epsilon$ is a small value near to zero.  

To avoid noises, choose the points whose incident faces have faces that are front-facing to the view direction and faces that are back-facing to the view direction. Among the silhouette candidates, the points sheltered by the nose has no edge on the 2D image, so these points will be discarded. To guarantee proper extraction of silhouette from the images, only the "nonfrontal" images are used. From the rotation matrix, we can estimate the yaw, pitch and roll of the face pose for each image. Choose the images in which the yaw of the face is bigger than a threshold. The points satisfying the above constraints are considered as silhouette  candidates on 3D model, we denote it as $X_{sil3D}$.


%------------------------------------------------------------------------
\subsection{Build correspondence between 3D silhouette and 2D silhouette}


Let $S_{i}$ denotes the silhouette candidates of 3D model for image $i$. Warp $S_{i}$ on image $i$.
Warp the silhouette candidate $X_{sil3D}$ on the images and move the candidates to the nearest points on an edge in the image. From the edge of one image, for each silhouette candidates on the 3D face model  $X_{sil3D}^{i}$ , find the closest  edge points by minimizing the square distance between warped silhouette point and edge points, i.e.,
\begin{displaymath}
\arg{\min}\underset{X_{sil2D}} \|X_{sil3D}^{i} - X_{sil2D}^i\|^2 
\end{displaymath} 

Since the reconstructed model is close to the real shape of the individual, the projected silhouette should not be too far away from the correspongding edge. We set a threshold for the square distance, if the distance is larger than the threshold, that point will be considered as outlier and rejected. This can also reduce the effect by the extreme expession.

For each image, we obtain the silhouette point sets and the corresponding vertex on 3D mesh. Then for each silhouette point on 3D model, we can know it's related to which images. Let $X_{j}$ denotes the silhouette vertex $j$, $I_{j}$ denotes the image set containing $X_{j}$ . $I_{ij}$ denotes image $i$ containing silhouette points correponding to $X_{j}$ . 


In our apporach, the face model is reconstructed from the average normals of images. The silhouette points that only appear in few images may lead to pointy region on 3D mesh. Especially for the chin, the extreme expression such as big mouth, may result in a bad estimation of the shape $X$. So the points showing in few images will be discarded to decrease the influence of noise in the slihouette extraction and the various expressions. 


% All text with the exception of the abstract must be in a two-column format.
% The total allowable width of the text area -- including header and footer
% lines -- is 161\,mm (6.34 inch) wide by 231\,mm (9.10 inch) high.
% 
% Columns are to be 76\,mm (3.0 inch) wide, with a 8\,mm (0.315 inch) space 
% between them.

%-------------------------------------------------------------------------
\subsection{Reconstruction using silhouette points}

Given the silhouette points on 3D model and the corresponding sihouette in 2D images, we add the silhouette constraint to the energy mentioned in [1] and we can get:
\begin{equation}
\begin{split}
E&=\|LX_{new} - H^{k}n\|^2+ \lambda_{l}\sum_{i}{\|P_{i}D_{i}X_{new}-W_{i}\|^2} \\
&+\lambda_{b}\|L_{b}X_{new} - LX\|^2+\lambda_{s}\sum_{j\in V_{sil}} \sum_{s\in I_{sil}}{\|P_{sj}D_{sj}X_{new}-S_{sj}\|^2}
\end{split}
\end{equation} 

where $I_{sil}$ is the image set containing silhouette points, $V_{sil}$ is all the silhouette vertex on 3D model. $P_{sj}$  is weakly prospective projected matrix between  silhouette vertices $X_{j}$ and 2D silhouette points in $s$ -th images, $D_{sj}$  is a diagonal matrix selecting out the silhouette vertex that have a correspondence in image $s$, . $\lambda_{s}$ is the silhouette constraint weight.

$X$ can be solved in a linear system as: 

\begin{equation}
\begin{split}
&(\mathcal{L}^2+\lambda_{b}\mathcal{L}_b^2 +\lambda_{l}\sum_{i}{D_{i}P_{i}^{T}P_{i}D_{i}}+\lambda_{s}\sum_{j} \sum_{s}{D_{sj}P_{sj}^{T}P_{sj}D_{sj}})X\\
&=\mathcal{L}H+\lambda_{b}\mathcal{L}_b^2X+\lambda_{l}\sum_{i}{(P_{i}^T)W_{i}}+\lambda_{s}\sum_{j}\sum_{s}{P_{sj}^{T}D_{sj}S_{sj}}
\end{split}
\end{equation} 



%-------------------------------------------------------------------------
\section{Experiment}
\label{sec:results}

In this section we present the visualization result of the improvement of our algorithm.

\textbf{Dataset} We collect the photo of celebrities on the website. First we tried to use Google API to collect the photo by searching celebrities' name, but less than half of the result can be use for the experiment, since many of the returned images are not portrait or are related to the celebrity. So we collect the images on the websites that own specific webpage for the celebrities, Mtime and Douban. And we write a python script to download images for different individuals. For the initial template used to reconstruct the model, we use the face model in \cite{Zhang04}. The landmarks are detected using \cite{kazemi2014one}

Extract silhouette points in a number of images of the dataset. Geoge Clooney(169/1152),  Kevin Spacey(85/468), Edward Norton(93/707), Tom Cruise(95/695) and James McAvoy(149/1038), WentworthMiller(106/737). The former number is the images used for extracting silhouette points, the latter number is the total amount of images for each celebrity.

\textbf{Silhouette}
\begin{figure}[htb]
  \centering
  % the following command controls the width of the embedded PS file
  % (relative to the width of the current column)
  \includegraphics[width=1\linewidth]{ite2}
  % replacing the above command with the one below will explicitly set
  % the bounding box of the PS figure to the rectangle (xl,yl),(xh,yh).
  % It will also prevent LaTeX from reading the PS file to determine
  % the bounding box (i.e., it will speed up the compilation process)
  % \includegraphics[width=1.5\linewidth, bb=39 696 126 756]{sampleFig}
  %
  %\parbox[t]{.9\columnwidth}{\relax
   %        For all figures please keep in mind that you \textbf{must not}
    %       use images with transparent background! 
     %      }
  %
  \caption{\label{fig:firstExample}
           the warped silhouette in each iteration. Red: warped silhouette. Green: extracted 2D silhouette}
\end{figure}

\begin{figure}[htb]
  \centering
  \includegraphics[width=1\linewidth]{sil2}
  % \includegraphics[width=1.5\linewidth]{sil}
  %
  %\parbox[t]{.9\columnwidth}{\relax
   %        For all figures please keep in mind that you \textbf{must not}
    %       use images with transparent background! 
     %      }
  %
  \caption{\label{fig:firstExample}
           the warped silhouette before and after using silhouette constraint. Red: before using silhouette. Yellow: after adding silhouette constraint}
\end{figure}

Fig.2 plots the warped silhouette points on the images. We can see that the after adding silhouette constraint to the reconstruction step, the silhouette of the 3D mesh (yellow points) match better with the real silhouette.

From the result in Fig.3 we can see, compared with the previous method, our result has obvious improvement on the nose and the chin. Also the eyes in the profile. When the 3D model has good correspondence to the images, it helps the photometric stereo approach to estimate more accurate normals. 


%-------------------------------------------------------------------------
\section{Conclusions}
\label{sec:conclusions}

Please direct any questions to the production editor in charge of
these proceedings.

%-------------------------------------------------------------------------


%-------------------------------------------------------------------------
\newpage

%\bibliographystyle{eg-alpha}
\bibliographystyle{eg-alpha-doi}

\bibliography{egbibsample}

\begin{figure*}[tbp]
  \centering
  %\mbox{} \hfill
  % the following command controls the width of the embedded PS file
  % (relative to the width of the current column)
  \includegraphics[width=.9\linewidth]{all2}
  % replacing the above command with the one below will explicitly set
  % the bounding box of the PS figure to the rectangle (xl,yl),(xh,yh).
  % It will also prevent LaTeX from reading the PS file to determine
  % the bounding box (i.e., it will speed up the compilation process)
  % \includegraphics[width=.3\linewidth, bb=39 696 126 756]{sampleFig}
  %
  \caption{\label{fig:ex3}%
           For publications with color tables (i.e., publications not offering
           color throughout the paper) please \textbf{observe}: 
           for the printed version -- and ONLY for the printed
           version -- color figures have to be placed in the last page.
           \newline
  }
\end{figure*}

\end{document}
