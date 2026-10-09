#ifndef XGP_ANIMWINDOW_H
#define XGP_ANIMWINDOW_H

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.

#include <sigc++/connection.h>
#include <sigc++/trackable.h>

namespace Gtk {
class Fixed;
class Widget;
} // namespace Gtk

namespace XGP {

/**Class animating a widget inside a Gtk::Fixed: The widget is moved in
 * several steps from its current position to the one returned by getEndPos().
 *
 * \note Create on heap (with new) as this class deletes itself when
 *       the animation has been finished (or the widget is destroyed).
 */
class AnimatedWindow : public sigc::trackable {
  public:
    virtual ~AnimatedWindow();

    void animate();

    /// Retrieves the position where to animate the widget to
    /// \param x X-coordinate of end-position (relative to the Gtk::Fixed)
    /// \param y Y-coordinate of end-position (relative to the Gtk::Fixed)
    virtual void getEndPos(double& x, double& y) = 0;
    virtual void start();
    virtual void cleanup();
    virtual void finish();

  protected:
    AnimatedWindow(Gtk::Fixed& parent, Gtk::Widget& widget);

    void animateTo(double x, double y);

    Gtk::Fixed& fixed;
    Gtk::Widget& widget;

  private:
    AnimatedWindow();
    AnimatedWindow(const AnimatedWindow& other);
    const AnimatedWindow& operator=(const AnimatedWindow& other);

    bool animationStep();
    void end();

    unsigned int steps;
    sigc::connection connTimer;
};

} // namespace XGP

#endif
