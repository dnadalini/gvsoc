import gvsoc.systree
import memory.memory as memory
import vp.clock_domain
import utils.loader.loader
import interco.router as router
import gvsoc.runner

from pulp.chips.archytas.archytas_A_tile import Archytas_A_Tile
from pulp.chips.archytas.archytas_arch import ArchytasArch
from pulp.floonoc.floonoc import *
from pulp.fractal_sync.fractal_sync import *

class A_Tile_SoC(gvsoc.systree.Component):
    def __init__(self, parent, name, parser, binary):
        super().__init__(parent, name)

        loader=utils.loader.loader.ElfLoader(self, 'loader', binary=binary)

        # Single clock domain
        clock = vp.clock_domain.Clock_domain(self, 'tile-clock', frequency=ArchytasArch.TILE_CLK_FREQ)
        clock.o_CLOCK(self.i_CLOCK())

        # Instantiate tile
        tile = Archytas_A_Tile(self, 'a-tile', parser, 0)

        # L2 memory
        l2_mem = memory.Memory(self, 'l2-mem', size=ArchytasArch.L2_SIZE, latency=1)

        # NoC
        noc = FlooNoc2dMeshNarrowWide(self,
                                    name=f'archytas-noc',
                                    narrow_width=4,
                                    wide_width=4,
                                    ni_outstanding_reqs=8,
                                    router_input_queue_size=2,
                                    dim_x=2,
                                    dim_y=1)

        # NoC routers
        noc.add_router(0, 0)
        noc.add_network_interface(0, 0)
        noc.add_router(1, 0)
        noc.add_network_interface(1, 0)

        id = 0

        tile.o_NARROW_OUTPUT(noc.i_NARROW_INPUT(1, 0))
        noc.o_NARROW_MAP(tile.i_NARROW_INPUT(), name=f'tile-{id}-narrow', base=ArchytasArch.L1_ADDR_START + id * ArchytasArch.L1_TILE_OFFSET, size=ArchytasArch.L1_SIZE, x=1, y=0, rm_base=False)

        noc.o_NARROW_MAP(l2_mem.i_INPUT(), name='l2-map', base=ArchytasArch.L2_ADDR_START, size=ArchytasArch.L2_SIZE, x=0, y=0, rm_base=True)

        # Bind loader
        loader.o_OUT(tile.i_LOADER())
        loader.o_START(tile.i_FETCHEN())
        loader.o_ENTRY(tile.i_ENTRY())

class ATileBoard(gvsoc.systree.Component):
    def __init__(self, parent, name:str, parser, options):
        super().__init__(parent, name, options=options)

        [args, __] = parser.parse_known_args()
        binary = args.binary

        # Soc model
        soc = A_Tile_SoC(self, 'a-tile-soc', parser, binary)

class Target(gvsoc.runner.Target):
    def __init__(self, parser, options):
        super(Target, self).__init__(parser, options,
              model=ATileBoard, description="Archytas A-Tile testbench")
