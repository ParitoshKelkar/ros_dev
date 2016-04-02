import os
import rospy
import numpy

from qt_gui.plugin import Plugin
from python_qt_binding import loadUi
from python_qt_binding.QtGui import QWidget
from std_msgs.msg import String
from std_msgs.msg import Int32
from rospy.numpy_msg import numpy_msg
from rospy_tutorials.msg import Floats

class customerPlugin(Plugin):
    
    def __init__(self, context):
        super(customerPlugin, self).__init__(context)
        # Give QObjects reasonable names
        self.setObjectName('customerPluginObject')
        self.exp_flag = 1
        self.s = "Y"
        #print _exp_flag
        # Process standalone plugin command-line arguments
        from argparse import ArgumentParser
        parser = ArgumentParser()
        # Add argument(s) to the parser.
        parser.add_argument("-q", "--quiet", action="store_true",
                      dest="quiet",
                      help="Put plugin in silent mode")
        args, unknowns = parser.parse_known_args(context.argv())
        if not args.quiet:
            print 'arguments: ', args
            print 'unknowns: ', unknowns

        self.pub = rospy.Publisher("PANDU_to_operator",Int32,queue_size=10);
        self.numpy_pub = rospy.Publisher("menu_num_pub", numpy_msg(Floats), queue_size=10)

        # Create QWidget
        self._widget = QWidget()
        # Get path to UI file which is a sibling of this file
        # in this example the .ui and .py file are in the same folder
        ui_file = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'customerView.ui')
        # Extend the widget with all attributes and children from UI file
        loadUi(ui_file, self._widget)
        # Give QObjects reasonable names
        self._widget.setObjectName('customerPluginUi')
        self._widget.pushButton_placeOrder.clicked.connect(self._click_order)
        self._widget.pushButton_Bill.clicked.connect(self._click_Bill)
        self._widget.pushButton_Ketchup.clicked.connect(self._click_Ketchup)
        self._widget.pushButton_unload.clicked.connect(self._click_unload)
        self._widget.pushButton_assist.clicked.connect(self._click_assist)
        # Show _widget.windowTitle on left-top of each plugin (when 
        # it's set in _widget). This is useful when you open multiple 
        # plugins at once. Also if you open multiple instances of your 
        # plugin at once, these lines add number to make it easy to 
        # tell from pane to pane.
        if context.serial_number() > 1:
            self._widget.setWindowTitle(self._widget.windowTitle() + (' (%d)' % context.serial_number()))
        # Add widget to the user interface
        context.add_widget(self._widget)
        rospy.set_param('TableNum',3)
        TableNum = rospy.get_param('TableNum')
    def shutdown_plugin(self):
        # TODO unregister all publishers here
        pass

    def _click_order(self):
        # check if that button press can be handled            
        item1_num = self._widget.spinBox1.value()
        item2_num = self._widget.spinBox2.value()
        item3_num = self._widget.spinBox3.value()            
        self.pub.publish(0)
        a = numpy.array([item1_num, item2_num, item3_num], dtype=numpy.float32)
        self.numpy_pub.publish(a)
        self._widget.plainTextEdit.setPlainText("Order has been placed!")
        
        pass

    def _click_unload(self):
        # the unload button
        self._widget.plainTextEdit.setPlainText("Bye!")
        print "done unloading"
       # rospy.sleep(2)
        self.pub.publish(777)
        pass

    def _click_assist(self):
        # the assisst button
        self._widget.plainTextEdit.setPlainText("I'll go tell the human!")
        print "assistance needed"
        self.pub.publish(999)
        pass               



    def _click_Bill(self):
        # Bill press
        print "Customer Asked For Bill"
        self._widget.plainTextEdit.setPlainText("I'll go get that for you!")        
        self.pub.publish(2)
        pass

    def _click_Ketchup(self):
        # Asked for Ketchup
        print "Asked for Ketchup"
        self._widget.plainTextEdit.setPlainText("I'll go get that for you!")        
        self.pub.publish(1)
        pass

    def save_settings(self, plugin_settings, instance_settings):
        # TODO save intrinsic configuration, usually using:
        # instance_settings.set_value(k, v)
        pass

    def restore_settings(self, plugin_settings, instance_settings):
        # TODO restore intrinsic configuration, usually using:
        # v = instance_settings.value(k)
        pass

    #def trigger_configuration(self):
        # Comment in to signal that the plugin has a way to configure
        # This will enable a setting button (gear icon) in each dock widget title bar
        # Usually used to open a modal configuration dialog

