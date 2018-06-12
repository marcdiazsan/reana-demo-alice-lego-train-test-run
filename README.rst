===========================================
 REANA example - ALICE LEGO train test run
===========================================

.. image:: https://img.shields.io/travis/reanahub/reana-demo-alice-lego-train-test-run.svg
   :target: https://travis-ci.org/reanahub/reana-demo-alice-lego-train-test-run

.. image:: https://badges.gitter.im/Join%20Chat.svg
   :target: https://gitter.im/reanahub/reana?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge

.. image:: https://img.shields.io/github/license/reanahub/reana-demo-alice-lego-train-test-run.svg
   :target: https://raw.githubusercontent.com/reanahub/reana-demo-alice-lego-train-test-run/master/LICENSE

About
=====

This `REANA <http://www.reanahub.io/>`_ reproducible analysis example performs
ALICE LEGO train test run and validation. The procedure is used in ALICE
collaboration particle physics analyses. Please see `arXiv:1502.06381
<https://arxiv.org/abs/1502.06381>`_ for more detailed description of the ALICE
analysis train system.

Analysis structure
==================

Making a research data analysis reproducible means to provide "runnable recipes"
addressing (1) where the input datasets are, (2) what software was used to
analyse the data, (3) which computing environment was used to run the software,
and (4) which workflow steps were taken to run the analysis.

1. Input data
-------------

This example uses ALICE proton-proton data sample input files. You will need to
take some Pb-Pb ESD file from `CERN Open Data portal
<http://opendata.cern.ch/>`_. For example, the following sample taken at 3.5 TeV
from RunB in 2010: (beware, the file is 231 MB large)

.. code-block:: console

   $ mkdir -p __alice__data__2011__LHC11h_2__000170387
   $ cd __alice__data__2011__LHC11h_2__000170387
   $ wget http://opendata.cern.ch/record/1100/files/assets/alice/2010/LHC10b/000117222/ESD/0001/AliESDs.root
   $ cd ..

Note that ``data.txt`` file should contain the path to the downloaded sample
data file.

2. Analysis code
----------------

This example uses the `AliPhysics <https://github.com/alisw/AliPhysics>`_
analysis framework with the following source code files:

- `env.sh <env.sh>`_ - ALICE LEGO train system configuration
- `generate.C <generate.C>`_ - a macro to generate macros to run the ALICE LEGO train
- `globalvariables.C <globalvariables.C>`_ - global variables
- `handlers.C <handlers.C>`_ - data access handlers (ESD/AOD, collision/MC)
- `runTest.sh <runTest.sh>`_ - run LEGO train test and validation
- `MLTrainDefinition.cfg <MLTrainDefinition.cfg>`_ - train wagon definitions
- `plot.C <plot.C>`_ - plot sample histogram

The user provides notably the `MLTrainDefinition.cfg <MLTrainDefinition.cfg>`_
file which defines a set of train wagons that compose the analysis train run. In
this example, the following wagons are defined:

.. code-block:: console

   $ grep Begin MLTrainDefinition.cfg
   #Module.Begin        PhysicsSelectionESD
   #Module.Begin        AliVZEROEPSelectionTask
   #Module.Begin        AliEPSelectionTask
   #Module.Begin        CentralityTaskESD
   #Module.Begin        PIDResponseTaskESD
   #Module.Begin        GammaConv_PurityTemplate_C_0

The first wagons are related to centralised data selection tasks, while the main
user analysis is executed in the last ``GammaConv`` wagon.

The ``runTest.sh`` script will take care of creating the train test run, running
it, and validating its outputs.

3. Compute environment
----------------------

This example uses `AliPhysics <https://github.com/alisw/AliPhysics>`_ analysis
framework. It has been containerised as `reana-env-aliphysics
<https://github.com/reanahub/reana-env-aliphysics>`_ environment. You can fetch
some wanted AliPhysics version from Docker Hub:

.. code-block:: console

   $ docker pull reanahub/reana-env-aliphysics:vAN-20170521-1

We shall use the ``vAN-20170521-1`` version for the present example.

Note that if you would like to build a different AliPhysics version on your own,
you can follow `reana-env-aliphysics
<https://github.com/reanahub/reana-env-aliphysics>`_ procedures and set
``ALIPHYSICS_VERSION`` environment variable appropriately:

.. code-block:: console

   $ cd src/reana-env-aliphysics
   $ export ALIPHYSICS_VERSION=vAN-20180521-1
   $ make build

4. Analysis workflow
--------------------

The researcher typically uses a single test run command:

.. code-block:: console

   $ ./runTest.sh

which performs all the tasks related to the analysis train generation, running
and validation. Underneath, the following sequence of commands is called:

.. code-block:: shell

   # generate the LEGO train run and validation files:
   aliroot -b -q generate.C > generation.log

   # perform the LEGO train test run:
   source ./lego_train.sh > stdout 2> stderr

   # verify that the expected result files are well present:
   source ./lego_train_validation.sh > validation.log

The produced log files indicate whether the train test run was successful and
whether the output is validated.

The computational workflow is therefore essentialy sequential in nature. We can
use the REANA serial workflow engine and represent the analysis workflow as
follows:

.. code-block:: text

              START
               |
               |
               V
   +----------------------------------------+
   | (1) download ESD input data file       |
   |                                        |
   |    $ wget http://opendata.cern.ch/...  |
   +----------------------------------------+
               |
               | ALIESD.root
               V
   +----------------------------------------+       +-------------------------+
   | (2) generate LEGO train files          |       |       input code        |
   |                                        |  <----|   MLTrainDefinition.cfg |
   |    $ aliroot -b -q generate.C          |       |   env.sh handlers.C ... |
   +----------------------------------------+       +-------------------------+
               |
               | lego_train.sh
               | lego_train_validation.sh
               | ...
               V
   +----------------------------------------+
   | (3) perform LEGO train test run        |
   |                                        |
   |    $ source ./lego_train.sh            |
   +----------------------------------------+
               |
               |  stdout
               | GammaConvFlow_69.root
               | ...
               V
   +----------------------------------------+
   | (4) validate test run outputs          |
   |                                        |
   |    $ source ./lego_train_validation.sh |
   +----------------------------------------+
               |
               | validation.log
               | GammaConvFlow_69.root
               V
   +----------------------------------------+
   | (5) plot sample histogram              |
   |                                        |
   |    $ root -b -q plot.C                 |
   +----------------------------------------+
               |
               | plot.pdf
               V
              STOP

We shall see below how this sequence of commands is represented for the REANA
serial workflow engine.

5. Output results
-----------------

The output of the ALICE LEGO analysis train test run and validation is available
in the ``stdout`` file. The success or failure is reported at the end:

.. code-block:: console

   $ tail -4 stdout
   * ----------------------------------------------------*
   * ----------------   Job Validated  ------------------*
   * ----------------------------------------------------*
   *******************************************************

The test run will also create `ROOT <https://root.cern.ch/>`_ output files that
usually contain histograms.

.. code-block:: console

   $ ls -l GammaConvFlow_69.root EventStat_temp.root
   -rw-r--r-- 1 root root 999737 May 30 17:35 EventStat_temp.root
   -rw-r--r-- 1 root root 273102 May 30 17:35 GammaConvFlow_69.root

The user typically uses the output files to produce final plots. For example,
running ``plot.C`` output macro on the ``GammaConvFlow_69.root`` output file
will permit to visualise a sample event plane histogram:

.. figure:: https://raw.githubusercontent.com/reanahub/reana-demo-alice-lego-train-test-run/master/docs/plot.png
   :alt: plot.png
   :align: center

Local testing
=============

*Optional*

If you would like to test the analysis locally (i.e. outside of the REANA
platform), you can proceed as follows:

.. code-block:: console

   $ docker run -i -t --rm -v `pwd`:/inputs \
        reanahub/reana-env-aliphysics:vAN-20170521-1 \
        'cd /inputs && source ./runTest.sh'
   $ tail -4 stdout
   $ ls -l GammaConvFlow_69.root EventStat_temp.root
   $ docker run -i -t --rm -v `pwd`:/inputs \
        reanahub/reana-env-aliphysics:vAN-20170521-1 \
        'cd /inputs && root -b -q ./plot.C'
   $ ls -l plot.pdf

Running the example on REANA cloud
==================================

First we need to create a `reana.yaml <reana.yaml>`_ file describing the
structure of our analysis with its inputs, the code, the runtime environment,
the workflow and the expected outputs:

.. code-block:: yaml

   version: 0.3.0
   inputs:
     files:
      - MLTrainDefinition.cfg
      - data.txt
      - env.sh
      - generate.C
      - globalvariables.C
      - handlers.C
      - plot.C
      - runTest.sh
      - fix-env.sh
     parameters:
       none: none
   outputs:
     files:
      - plot.pdf
   environments:
    - type: docker
      image: reanahub/reana-env-aliphysics:vAN-20170521-1
   workflow:
     type: serial
     specification:
       steps:
         - environment: 'reanahub/reana-env-aliphysics:vAN-20170521-1'
           commands:
           - 'cp ../inputs/* .'
           - 'mkdir __alice__data__2011__LHC11h_2__000170387/'
           - 'wget http://opendata.cern.ch/record/1100/files/assets/alice/2010/LHC10b/000117222/ESD/0001/AliESDs.root'
           - 'mv AliESDs.root __alice__data__2011__LHC11h_2__000170387/'
           - 'source fix-env.sh && source env.sh && aliroot -b -q generate.C > generation.log 2> generation.err'
           - 'source fix-env.sh && source env.sh && export ALIEN_PROC_ID=12345678 && source ./lego_train.sh > stdout 2> stderr'
           - 'source fix-env.sh && source env.sh && source ./lego_train_validation.sh > validation.log 2> validation.err'
           - 'source fix-env.sh && source env.sh && root -b -q ./plot.C'

We proceed by installing the REANA command-line client:

.. code-block:: console

    $ mkvirtualenv reana-client
    $ pip install reana-client

We should now connect the client to the remote REANA cloud where the analysis
will run. We do this by setting the ``REANA_SERVER_URL`` environment variable:

.. code-block:: console

    $ export REANA_SERVER_URL=https://reana.cern.ch/

Note that if you `run REANA cluster locally
<http://reana-cluster.readthedocs.io/en/latest/gettingstarted.html#deploy-reana-cluster-locally>`_
on your laptop, you would do:

.. code-block:: console

   $ eval $(reana-cluster env)

Let us test the client-to-server connection:

.. code-block:: console

   $ reana-client ping
   Server is running.

We can now seed the analysis workspace with input files:

.. code-block:: console

   $ reana-client inputs upload MLTrainDefinition.cfg data.txt \
        env.sh generate.C globalvariables.C handlers.C plot.C \
        runTest.sh fix-env.sh

We can now start the workflow execution:

.. code-block:: console

    $ reana-client workflow start
    workflow.1 has been started.

After several minutes the workflow should be successfully finished. Let us query
its status:

.. code-block:: console

    $ reana-client workflow status
    NAME       RUN_NUMBER   ID                                     USER                                   ORGANIZATION   STATUS
    workflow   1            0df60c85-9d84-402e-814c-0595fe5fd439   00000000-0000-0000-0000-000000000000   default        finished

We can list and download the output files:

.. code-block:: console

   $ reana-client outputs list
   $ reana-client outputs download stdout
   $ reana-client outputs download plot.pdf

Contributors
============

The list of contributors in alphabetical order:

- Markus Zimmermann <m.zimmermann@cern.ch>
- `Tibor Simko <https://orcid.org/0000-0001-7202-5803>`_ <tibor.simko@cern.ch>
