/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-11 13:59:30 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-11 13:59:30 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "HttpResponse.hpp"

class AutoIndex {
   private:
    std::string generateRow(std::string name, const struct stat& info);
    std::string formatSize(off_t size);
    std::string formatTime(time_t time);

   public:
    int generate(const std::string& path, const std::string& uri, std::string& body);
};
