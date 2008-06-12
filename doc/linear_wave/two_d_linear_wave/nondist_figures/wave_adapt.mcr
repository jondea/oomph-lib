#!MC 1000

$!VARSET |PNG|=0


#$!GETUSERINPUT |lostep| INSTRUCTIONS = "Loop. First Step??"
$!VARSET  |lostep|=0
#$!GETUSERINPUT |dlstep| INSTRUCTIONS = "Loop. Step Increment?"
$!VARSET  |dlstep|=2
$!GETUSERINPUT |nstep| INSTRUCTIONS = "Loop. Number of Steps??"
#$!VARSET  |nstep|=400

$!LOOP |nstep|
$!VarSet |nnstep| = |LOOP|
$!VarSet |nnstep| -= 1
$!VarSet |iistep| = |dlstep|
$!VarSet |iistep| *= |nnstep|
$!VarSet |iistep| += |lostep|
$!NEWLAYOUT
$!DRAWGRAPHICS FALSE
#    $!IF |iistep| < 10 
#      $!VARSET |istep|='00|iistep|'
#    $!ENDIF
#    $!IF |iistep| > 9 
#      $!VARSET |istep|='0|iistep|'
#    $!ENDIF
#    $!IF |iistep| > 99 
#      $!VARSET |istep|=|iistep|
#    $!ENDIF
$!VARSET |istep|=|iistep|
#$!VARSET |istep|+=1
#$!VARSET |istep|*=10

$!DRAWGRAPHICS FALSE

$!VarSet |LFDSFN1| = '"RESLT/soln|istep|.dat" "RESLT/exact_soln|istep|.dat"'
$!VarSet |LFDSVL1| = '"V1" "V2" "V3"'
$!SETSTYLEBASE FACTORY
$!PAPER 
  BACKGROUNDCOLOR = WHITE
  ISTRANSPARENT = YES
  ORIENTPORTRAIT = NO
  SHOWGRID = YES
  SHOWRULER = YES
  SHOWPAPER = YES
  PAPERSIZE = A4
  PAPERSIZEINFO
    {
    A3
      {
      WIDTH = 11.693
      HEIGHT = 16.535
      }
    A4
      {
      WIDTH = 8.2677
      HEIGHT = 11.693
      LEFTHARDCLIPOFFSET = 0.125
      RIGHTHARDCLIPOFFSET = 0.125
      TOPHARDCLIPOFFSET = 0.125
      BOTTOMHARDCLIPOFFSET = 0.125
      }
    }
  RULERSPACING = ONECENTIMETER
  PAPERGRIDSPACING = ONETENTHCENTIMETER
  REGIONINWORKAREA
    {
    X1 = -0.05
    Y1 = -0.05
    X2 = 11.74
    Y2 = 8.318
    }
$!COLORMAP 
  CONTOURCOLORMAP = SMRAINBOW
$!COLORMAPCONTROL RESETTOFACTORY
### Frame Number 1 ###
$!READDATASET  '|LFDSFN1|' 
  INITIALPLOTTYPE = CARTESIAN3D
  INCLUDETEXT = YES
  INCLUDEGEOM = YES
  VARLOADMODE = BYNAME
  VARNAMELIST = '|LFDSVL1|' 
$!REMOVEVAR |LFDSVL1|
$!REMOVEVAR |LFDSFN1|

$!VARSET |END1| = |NUMZONES|
$!VARSET |END1| -= 2
$!VARSET |END1| /= 2
$!VARSET |BOUND1| = |END1|
$!VARSET |BOUND1| += 1
$!VARSET |BOUND2| = |BOUND1|
$!VARSET |BOUND2| += 1
$!VARSET |START2| = |BOUND2|
$!VARSET |START2| += 1


$!RENAMEDATASETVAR 
  VAR = 1
  NAME = 'x' 
$!RENAMEDATASETVAR 
  VAR = 2
  NAME = 'y' 
$!RENAMEDATASETVAR 
  VAR = 3
  NAME = 'u(x,y,t)' 
$!FRAMELAYOUT 
  SHOWBORDER = NO
  SHOWHEADER = NO
  HEADERCOLOR = RED
  XYPOS
    {
    X = 0.3937
    Y = 0.3937
    }
  WIDTH = 10.906
  HEIGHT = 7.4803
$!PLOTTYPE  = CARTESIAN3D
$!FRAMENAME  = 'Frame 001' 
$!ACTIVEFIELDZONES  =  [1-|NUMZONES|]
$!GLOBALRGB 
  RANGEMIN = 0
  RANGEMAX = 1
$!GLOBALCONTOUR  1
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  2
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  3
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  4
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALSCATTER 
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  REFSCATSYMBOL
    {
    COLOR = RED
    FILLCOLOR = RED
    }
$!FIELD  [1-|END1|]
  MESH
    {
    SHOW = NO
    COLOR = RED
    }
  CONTOUR
    {
    SHOW = NO
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    SHOW = NO
    COLOR = RED
    }
  SCATTER
    {
    SHOW = NO
    COLOR = RED
    }
  SHADE
    {
    COLOR = CUSTOM50
    }
  BOUNDARY
    {
    SHOW = YES
    COLOR = RED
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }



$!FIELD  [|BOUND1|-|BOUND2|]
  MESH
    {
    COLOR = BLUE
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = BLUE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = BLUE
    }
  SHADE
    {
    SHOW = NO
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLUE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 3

$!FIELD  [|START2|-|NUMZONES|]
  MESH
    {
    SHOW = NO
    COLOR = YELLOW
    }
  CONTOUR
    {
    SHOW = NO
    CONTOURTYPE = FLOOD
    COLOR = YELLOW
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    SHOW = NO
    COLOR = YELLOW
    }
  SCATTER
    {
    SHOW = NO
    COLOR = YELLOW
    }
  SHADE
    {
    COLOR = CUSTOM35
    }
  BOUNDARY
    {
    SHOW = YES
    COLOR = GREEN
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2




$!THREEDAXIS 
  XDETAIL
    {
    VARNUM = 1
    }
  YDETAIL
    {
    VARNUM = 2
    }
  ZDETAIL
    {
    VARNUM = 3
    }
$!VIEW FIT
$!THREEDAXIS 
  AXISMODE = XYZDEPENDENT
  XYDEPXTOYRATIO = 1
  DEPXTOYRATIO = 1
  DEPXTOZRATIO = 1
$!THREEDAXIS 
  XDETAIL
    {
    RANGEMIN = -0.10499999839812516889
    RANGEMAX = 1.1049999499693512472
    GRSPACING = 0.2
    AXISLINE
      {
      EDGE = 2
      }
    }
$!THREEDAXIS 
  YDETAIL
    {
    RANGEMIN = -0.15499999839812517166
    RANGEMAX = 2.1549999499693512917
    GRSPACING = 0.5
    AXISLINE
      {
      EDGE = 3
      }
    }
$!THREEDAXIS 
  ZDETAIL
    {
    RANGEMIN = -1.1055755615234375
    RANGEMAX = 1.1002655029296875
    GRSPACING = 0.5
    AXISLINE
      {
      EDGE = 3
      }
    }
$!GLOBALISOSURFACE 
  MARCHINGCUBEALGORITHM = CLASSICPLUS
$!GLOBALSLICE 
  BOUNDARY
    {
    SHOW = NO
    }
$!GLOBALTHREED 
  AXISSCALEFACT
    {
    X = 1
    Y = 1
    Z = 1
    }
  ROTATEORIGIN
    {
    X = 0.499999975786
    Y = 0.999999975786
    Z = -0.00265502929688
    }
  LIGHTSOURCE
    {
    INTENSITY = 75
    BACKGROUNDLIGHT = 30
    }
  LINELIFTFRACTION = 0.2
  SYMBOLLIFTFRACTION = 0.6
  VECTORLIFTFRACTION = 0.7
$!THREEDVIEW 
  VIEWERPOSITION
    {
    X = 13.669627396
    Y = 14.169627396
    Z = 10.7503007314
    }
  VIEWWIDTH = 5.05958
$!FIELDLAYERS 
  SHOWMESH = NO
  SHOWSHADE = YES
  USETRANSLUCENCY = YES
$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 78.279207087
    Y = 64.4303790067
    }
  COLOR = GREEN
  TEXTSHAPE
    {
    HEIGHT = 18
    }
  BOX
    {
    MARGIN = 10
    LINETHICKNESS = 0.4
    }
  SCOPE = GLOBAL
  MACROFUNCTIONCOMMAND = '' 
  TEXT = 'Exact Solution' 
$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 78.279207087
    Y = 69.4910851807
    }
  COLOR = RED
  TEXTSHAPE
    {
    HEIGHT = 18
    }
  BOX
    {
    MARGIN = 10
    LINETHICKNESS = 0.4
    }
  SCOPE = GLOBAL
  MACROFUNCTIONCOMMAND = '' 
  TEXT = 'Computed Solution' 

$!SETSTYLEBASE CONFIG


############################


$!REDRAWALL


$!IF |PNG|==1


        $!EXPORTSETUP EXPORTFORMAT = PNG
        $!EXPORTSETUP IMAGEWIDTH = 600
        $!EXPORTSETUP EXPORTFNAME = 'wave_sol|LOOP|.png'
        $!EXPORT
          EXPORTREGION = CURRENTFRAME

        $!EXPORTSETUP EXPORTFORMAT = EPS
        $!EXPORTSETUP IMAGEWIDTH = 1423
        $!EXPORTSETUP EXPORTFNAME = 'wave_sol|LOOP|.eps'

        $!EXPORTSETUP PRINTRENDERTYPE = IMAGE
        $!EXPORTSETUP EXPORTFNAME = 'wave_sol|LOOP|.img.eps'
        $!EXPORT
          EXPORTREGION = CURRENTFRAME

$!ELSE

        $!IF |LOOP|>1
                $!EXPORTNEXTFRAME
        $!ELSE

                $!EXPORTSETUP
                 EXPORTFORMAT = AVI
                 EXPORTFNAME = "wave_sol.avi"
                $!EXPORTSETUP IMAGEWIDTH = 829
                $!EXPORTSTART
        $!ENDIF

$!ENDIF


$!VARSET |LAST_STEP|=|LOOP|

$!EndLoop


$!IF |PNG|==0
        $!EXPORTFINISH
$!ENDIF


$!QUIT

