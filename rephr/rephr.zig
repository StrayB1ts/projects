//  1. print menu to user with options (1: input edition and article, 2: open saved article, 3: quit)
//      1.1 ask user for edition and article
//           - ensure they are valid numbers (above 0)
//           - they exist on the site (poll site before getting input to check max edition/article available)
//      1.2 ask user for file to open
//           - ensure valid filepath is used
//           - prepend with special value to denote that it is an article and the user cant open a random file
// 2. parse site for selected article and print it
//      - try to implement vi-like navigation
//      - do some parsing and display special things like headings or links different from plain text
// 3. after reaching end of article ask user if they would like to save the article
//      - if yes create file and save to that otherwise return to menu
const std = @import("std");
const stdout = std.io.getStdOut().writer();
const stdin = std.io.getStdIn().reader();
pub fn main() !void{
}
